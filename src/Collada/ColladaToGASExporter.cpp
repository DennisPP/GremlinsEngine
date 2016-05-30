#include "ColladaToGASExporter.h"

#include "ColladaVisualScene.h"

#include "ColladaSource.h"
#include "ColladaModel.h"
#include "ColladaNode.h"
#include "ColladaController.h"
#include "ColladaSkin.h"
#include "ColladaMorph.h"
#include "ColladaMesh.h"
#include "ColladaTriangle.h"
#include "ColladaInstanceGeometry.h"
#include "ColladaGeometry.h"
#include "ColladaInput.h"
#include "ColladaSkin.h"


#include "GBO/GBOModel.h"
#include "GBO/GBOAnimation.h"


#include "GMO/GMOModel.h"


#include "Math3D/Vector2.h"
#include "Math3D/Vector3.h"


#include <map>
#include <set>
#include <stack>
#include <queue>

#include <cstdio>
#include "Graphics/FlatRenderer.h"


#include "MD5/MD5.h"

#pragma pack(push)
#pragma pack(1)



class TDataItem
{
public:
    Vector3 position;
    float u, v;
    int vertex;
    int weightStart;
    int weightCount;
};



TVertexItem::TVertexItem()
{
    vertex = -1;
    uv = -1;
}


/* THE MD5 uv is binded to the vertex as defined in MD5.h
this is different from how the uv is defined as a seperate list and an index to the uv list
the following function use to help to do the conversion.
class TMD5Vertex
{
public:
    
    float u, v;
    int startWeight;
    int weightCount;
    
    // vertex position in the bind pose
    Vector3 bindPoseVertexPosition;
};
*/

class TNodeInfo
{
public:
    TNodeInfo();
    TColladaNode* node;
    Matrix4 inverseBindPose;
    int parentId;
};

TNodeInfo::TNodeInfo()
{
    node = NULL;
    parentId = -1;
}

void TColladaToGASExporter::ExportGBAscill(TColladaModel* model, const char* filename)
{
    std::vector<TColladaAnimation*> animations;
    model->CollectAnimations(animations);
    
    if( animations.empty())
        return;
    
    FILE* fp = fopen(filename, "wb");
    assert( fp );
    
    
    TColladaVisualScene* visualScene = model->visualScenes[0];
    
    for(int j=0;j<visualScene->rootNodes.size();++j)
    {
        TColladaNode* node = visualScene->rootNodes[j];
        assert( node->IsRootNode());
        node->UpdateTransform();
    }
    
    std::map<int, TColladaNode*> jointMap;
    std::map<TColladaNode*, int> jointReverseMap;
    //std::map<std::string, std::string> jointSIDToIDMap;
    
    std::stack<TColladaNode*> st;
    
    
    int nodeId = 0;
    
    int treeCount = 0;
    for(int j=0;j<visualScene->allNodes.size();++j)
    {
        TColladaNode* node = visualScene->allNodes[j];
        int nodeId = 0;
        if( node->IsRootNode())
        {
            ++ treeCount;
            st.push(node);
            while(!st.empty())
            {
                TColladaNode* top = st.top();
                st.pop();
                if( top->type == std::string("JOINT"))
                {
                
                std::string& name = top->sid;
                Matrix4 invBindPose;
                bool found = model->GetInverseBindPoseByJointName(name, invBindPose);
                if( found )
                {
                    jointMap[nodeId] = top;
                    jointReverseMap[top] = nodeId;
                    
                    //jointSIDToIDMap[top->sid] = top->id;
                    
                    //inverseBindPoseMap[top] = invBindPose;
                    
                    ++ nodeId;
                }
                }
                for(int child=(int)top->childs.size()-1;child>=0;--child)
                {
                    st.push(top->childs[child]);
                }
                
            }
        }
    }
    
    //  assert( treeCount == 1 );
    
    
    
    
    
    fprintf(fp, "GBA version 01\n");
    
    
    
    float frameRate = 30.0f; // HACK
    fprintf(fp, "frameRate %f\n", frameRate);
    
    // write the joint count
    int jointCount = (int)jointMap.size();
    fprintf(fp, "numJoints %d\n", jointCount);
    
    
    // write the frame count;
    
    
    
    int numFrames = 0;
    for(size_t anim=0;anim<animations.size();++anim)
    {
        TColladaSource* timeSource = animations[anim]->GetTimeSource();
        if( timeSource )
        {
            numFrames = (int)timeSource->values.size();
            break;
        }
    }
    
    fprintf(fp, "numFrames %d\n", numFrames);
    
    std::map<int, TColladaNode*>::iterator it = jointMap.begin();
    
    PrepareAnimations(model, 0);
    
    int startIndex = 0;
    // write the joint data
    int numAnimatedComponens = 0;
    for(;it!=jointMap.end();++it)
    {
        TColladaNode* joint = it->second;
        std::map<TColladaNode*, std::vector<TAnimTransform>>::iterator it = nodeTransforms.find(joint);
        if( it != nodeTransforms.end())
        {
            if( it->second.size()==1 && it->second[0].type == TAnimTransform::Matrix )
            {
                numAnimatedComponens += 6;
            }
            else
            {
                for(size_t at=0;at<it->second.size();++at)
                {
                    ++ numAnimatedComponens;
                }
            }
        }
    }
    
   // fprintf(fp, "numAnimatedComponens %d\n", numAnimatedComponens);
    
    fprintf(fp, "hierarchy {\n");
    it = jointMap.begin();
    for(;it!=jointMap.end();++it)
    {
        TColladaNode* joint = it->second;
        int flags = 0;
        int items = 0;
        std::map<TColladaNode*, std::vector<TAnimTransform>>::iterator it = nodeTransforms.find(joint);
        if( it != nodeTransforms.end())
        {
            if( it->second.size()==1 && it->second[0].type == TAnimTransform::Matrix )
            {
                flags = 63;
                items = 6;
            }
            else
            {
                for(size_t at=0;at<it->second.size();++at)
                {
                    TAnimTransform anim = it->second[at];
                    flags += anim.type;
                    ++ items;
                }
            }
        }
        
        assert( joint->type == std::string("JOINT"));
        
        int sindex = startIndex;
        if( flags == 0 )
            sindex = 0;
        
        int parentId = -1;
        TColladaNode* parentJoint = joint->parentNode;
        if( parentJoint )
        {
            std::map<TColladaNode*, int>::iterator it = jointReverseMap.find(parentJoint);
            if( it != jointReverseMap.end())
            {
                assert( it != jointReverseMap.end());
                parentId = jointReverseMap[parentJoint];
                assert( parentId >= 0);
            }
        }
        
        Matrix4 mat = joint->baseTransform;
        Vector3 p = mat.getTrans();
        
        Matrix3 mat3;
        mat.extract3x3Matrix(mat3);
        Quaternion q;
        q.FromRotationMatrix(mat3);
        
        
        fprintf(fp, "\t %s %d ( %f %f %f ) ( %f %f %f )\n", joint->sid.c_str(), parentId, p.x, p.y, p.z, q.x, q.y, q.z);
        
        
        if( flags != 0 )
            startIndex += items;
        
    }
    fprintf(fp, "}\n");
    
    
    std::map<int, std::vector<TColladaAnimation*>> animationMap;
    for(int anim=0;anim<(int)animations.size();++anim)
    {
        TColladaAnimation* animation = animations[anim];
        std::string target = animation->target;
        size_t p = target.find_first_of("/");
        if( p != std::string::npos )
        {
            target = target.substr(0, p);
        }
        TColladaNode* joint = visualScene->FindNodeById(target);
        if( !joint )
            continue;
        assert( joint );
        
        std::map<TColladaNode*, int>::iterator it = jointReverseMap.find(joint);
        assert( it != jointReverseMap.end());
        
        
        
        
        
        //int length = (int)strlen(target.c_str());
        //fwrite(&length, sizeof(int), 1, fp);
        //fwrite(target.c_str(), length, 1, fp);
        
        
        animationMap[it->second].push_back(animation);
       
        
    }
    
    int animationCount = (int)animationMap.size();
    
    fprintf(fp, "numAnimations %d\n", animationCount);

    std::map<int, std::vector<TColladaAnimation*>>::iterator ait = animationMap.begin();
    for(;ait!=animationMap.end();++ait)
    {
        std::vector<TColladaAnimation*>& animations = ait->second;
       if( !animations.empty())
       {
           int frameCount = animations[0]->GetFrameCount();
           std::string target = animations[0]->target;
           size_t p = target.find_first_of("/");
           if( p != std::string::npos )
           {
               target = target.substr(0, p);
           }
           
           TColladaNode* node = visualScene->FindNodeById(target);
           std::string name = node->sid;
           
           fprintf(fp, "anim %s %d\n", name.c_str(), frameCount);
           for(int f=0;f<frameCount;++f)
           {
               Matrix4 transform;
               GetCombinedTransform(animations, f, transform);
               
               fprintf(fp, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n",
                       transform._m[0], transform._m[1],transform._m[2],transform._m[3],
                       transform._m[4], transform._m[5],transform._m[6],transform._m[7],
                       transform._m[8], transform._m[9],transform._m[10],transform._m[11],
                       transform._m[12], transform._m[13],transform._m[14],transform._m[15]
                       );
           }
       }
    }
    
    
    fclose(fp);
}



void TColladaToGASExporter::ExportGBA(TColladaModel* model,const char* filename)
{
    std::vector<TColladaAnimation*> animations;
    model->CollectAnimations(animations);
    if( animations.empty())
        return;
    
    FILE* fp = fopen(filename, "wb");
    assert( fp );
    
    
    TColladaVisualScene* visualScene = model->visualScenes[0];
    
    for(int j=0;j<visualScene->rootNodes.size();++j)
    {
        TColladaNode* node = visualScene->rootNodes[j];
        assert( node->IsRootNode());
        node->UpdateTransform();
    }
    
    std::map<int, TColladaNode*> jointMap;
    std::map<TColladaNode*, int> jointReverseMap;
    
    
    std::stack<TColladaNode*> st;
    
    int nodeId = 0;
    /*
    for(int j=0;j<visualScene->jointNodes.size();++j)
    {
        TColladaNode* node = visualScene->jointNodes[j];
        std::string& name = node->name;
        Matrix4 invBindPose;
        bool found = model->GetInverseBindPoseByJointName(name, invBindPose);
     
        if( found )
        {
            jointMap[nodeId] = node;
            jointReverseMap[node] = nodeId;
            ++ nodeId;
        }
    }
     */
    
    
    int treeCount = 0;
    
    for(int j=0;j<visualScene->allNodes.size();++j)
    {
        TColladaNode* node = visualScene->allNodes[j];
        int nodeId = 0;
        if( node->IsRootNode())
        {
            ++ treeCount;
            st.push(node);
            while(!st.empty())
            {
                TColladaNode* top = st.top();
                st.pop();
                
                std::string& name = top->id;
                Matrix4 invBindPose;
                bool found = model->GetInverseBindPoseByJointName(name, invBindPose);
                if( found )
                {
                    jointMap[nodeId] = top;
                    jointReverseMap[top] = nodeId;
                    
                    ++ nodeId;
                }
                for(int child=(int)top->childs.size()-1;child>=0;--child)
                {
                    st.push(top->childs[child]);
                }
            }
        }
    }
    
  //  assert( treeCount == 1 );
    
    
    
    TGBAHeader header;
    header.magic[0] = 'G';
    header.magic[1] = 'B';
    header.magic[2] = 'A';
    header.magic[3] = 0;
    
    header.version = 1;
    
    fwrite(&header, sizeof(header), 1 , fp);
    
    
    // write the joint count
    int jointCount = (int)jointMap.size();
    fwrite(&jointCount, sizeof(int), 1, fp);
    
    
    
    int numFrames = 0;

    for(size_t anim=0;anim<animations.size();++anim)
    {
        TColladaAnimation* animation = animations[anim];
        TColladaSource* timeSource = animation->GetTimeSource();
        if( timeSource )
        {
            numFrames = (int)timeSource->values.size();
            break;
        }
    }
    
    fwrite(&numFrames, sizeof(int), 1, fp);
    
    std::map<int, TColladaNode*>::iterator it = jointMap.begin();
    
    PrepareAnimations(model, 0);
    
    int startIndex = 0;
    // write the joint data
    int numAnimatedComponens = 0;
    for(;it!=jointMap.end();++it)
    {
        TColladaNode* joint = it->second;
        std::map<TColladaNode*, std::vector<TAnimTransform>>::iterator it = nodeTransforms.find(joint);
        if( it != nodeTransforms.end())
        {
            if( it->second.size()==1 && it->second[0].type == TAnimTransform::Matrix )
            {
                numAnimatedComponens += 6;
            }
            else
            {
                for(size_t at=0;at<it->second.size();++at)
                {
                    ++ numAnimatedComponens;
                }
            }
        }
    }    
    fwrite(&numAnimatedComponens, sizeof(int), 1, fp);

    it = jointMap.begin();
    for(;it!=jointMap.end();++it)
    {
        TColladaNode* joint = it->second;
        int flags = 0;
        int items = 0;
        std::map<TColladaNode*, std::vector<TAnimTransform>>::iterator it = nodeTransforms.find(joint);
        if( it != nodeTransforms.end())
        {
            if( it->second.size()==1 && it->second[0].type == TAnimTransform::Matrix )
            {
                flags = 63;
                items = 6;
            }
            else
            {
                for(size_t at=0;at<it->second.size();++at)
                {
                    TAnimTransform anim = it->second[at];
                    flags += anim.type;
                    ++ items;
                }
            }
        }
        
        assert( joint->type == std::string("JOINT"));
        
        int sindex = startIndex;
        if( flags == 0 )
            sindex = 0;
        
        int parentId = -1;
        TColladaNode* parentJoint = joint->parentNode;
        if( parentJoint )
        {
            std::map<TColladaNode*, int>::iterator it = jointReverseMap.find(parentJoint);
            if( it != jointReverseMap.end())
            {
                assert( it != jointReverseMap.end());
                parentId = jointReverseMap[parentJoint];
                assert( parentId >= 0);
            }
        }
        
        //fprintf(fp, "\t\"%s\" %d %d %d\n", joint->id.c_str(), parentId, flags, sindex);
                
        int nameLength = (int)joint->name.length();
        fwrite(&nameLength, sizeof(int), 1, fp);
        fwrite(joint->name.c_str(), nameLength, 1, fp);
        
        fwrite(&parentId, sizeof(int), 1, fp);
        fwrite(&flags, sizeof(int), 1, fp);
        fwrite(&startIndex, sizeof(int), 1, fp);
        
        
        Matrix4 mat = joint->baseTransform;
        
        Vector3 p = mat.getTrans();
        
        
        Matrix3 mat3;
        mat.extract3x3Matrix(mat3);
        Quaternion q;
        q.FromRotationMatrix(mat3);
        
        fwrite(&p.x, sizeof(float), 1, fp);
        fwrite(&p.y, sizeof(float), 1, fp);
        fwrite(&p.z, sizeof(float), 1, fp);
        fwrite(&q.x, sizeof(float), 1, fp);
        fwrite(&q.y, sizeof(float), 1, fp);
        fwrite(&q.z, sizeof(float), 1, fp);

        
        
        if( flags != 0 )
            startIndex += items;
        
    }
    
    int animationCount = (int)animations.size();
    fwrite(&animationCount, sizeof(int), 1, fp);
    
    
    for(int anim=0;anim<animationCount;++anim)
    {
        TColladaAnimation* animation = animations[anim];
        std::string target = animation->target;
        size_t p = target.find_first_of("/");
        if( p != std::string::npos )
        {
            target = target.substr(0, p);
        }

        int length = (int)strlen(target.c_str());
        fwrite(&length, sizeof(int), 1, fp);
        fwrite(target.c_str(), length, 1, fp);
        
        std::vector<Matrix4> transforms;
        GetAnimationTransfrom(animation, transforms);
        int transformCount = (int)transforms.size();
        fwrite(&transformCount, sizeof(int), 1, fp);
        for(int j=0;j<(int)transforms.size();++j)
        {
            for(int k=0;k<16;++k)
                fwrite(&transforms[j]._m[k], sizeof(float), 1, fp);
        }
    }
    
    
    
    /*
    // write the base frame
    it = jointMap.begin();
    for(;it!=jointMap.end();++it)
    {
        TColladaNode* joint = it->second;
        Matrix4 transform = joint->baseTransform;
        if( joint->parentNode )
        {
            transform = transform * joint->parentNode->transform.inverse();
        }
        
        Vector3 pos = transform.getTrans();
        
        Matrix3 mat3;
        
        transform.extract3x3Matrix(mat3);
        Quaternion q;
        q.FromRotationMatrix(mat3);
        //q = q.Inverse();
        q.normalise();
        
        
        fwrite(&pos.x, sizeof(float), 1, fp);
        fwrite(&pos.y, sizeof(float), 1, fp);
        fwrite(&pos.z, sizeof(float), 1, fp);
        fwrite(&q.x, sizeof(float), 1, fp);
        fwrite(&q.y, sizeof(float), 1, fp);
        fwrite(&q.z, sizeof(float), 1, fp);
        
    }
    
    
    // write the animation frames
    for(int f=0;f<numFrames;++f)
    {
       // fprintf(fp, "frame %d {\n", f);
        PrepareAnimations(model, f);
        //assert( numAnimatedComponents == 6 * (int)nodeTransforms.size());
        //for(int j=0;j<(int)visualScene->jointNodes.size();++j) // TODO: HACK
        
        it = jointMap.begin();
        for(;it!=jointMap.end();++it)
        {
          //  int nameIndex = jointToNameMap[it->second];
            //const std::string& jointName = jointNameSource->nameArray->names[nameIndex];
            
            TColladaNode* joint = it->second;
            // TColladaNode* joint = visualScene->jointNodes[j];
            
            
            
            assert( joint );
            std::map<TColladaNode*, std::vector<TAnimTransform>>::iterator it = nodeTransforms.find(joint);
            ///assert( it != nodeTransforms.end());
            if( it != nodeTransforms.end() )
            {
                for(size_t at=0;at<it->second.size();++at)
                {
                    TAnimTransform anim = it->second[at];
                    if( anim.type == TAnimTransform::Matrix )
                    {
                        float* m = anim.value.matrix;
                        Matrix4 mat(m[0], m[1], m[2], m[3],
                                    m[4], m[5], m[6], m[7],
                                    m[8], m[9], m[10], m[11],
                                    m[12], m[13], m[14], m[15]
                                    );
                        
                        
                        
                        //mat = mat * joint->transform;
                        Matrix3 mat3;
                        mat.extract3x3Matrix(mat3);
                        Quaternion q;
                        q.FromRotationMatrix(mat3);
                        q = q.Inverse();
                        q.normalise();
                        
                        float x = mat.getTrans().x;
                        float y = mat.getTrans().y;
                        float z = mat.getTrans().z;
                        
                        fwrite(&x, sizeof(float), 1, fp);
                        fwrite(&y, sizeof(float), 1, fp);
                        fwrite(&z, sizeof(float), 1, fp);
                        fwrite(&q.x, sizeof(float), 1, fp);
                        fwrite(&q.y, sizeof(float), 1, fp);
                        fwrite(&q.z, sizeof(float), 1, fp);
                        
                       // fprintf(fp, "\t %f %f %f %f %f %f", x, y, z, q.x, q.y, q.z);
                    }
                    else
                    {
                        //fprintf(fp, "%f", anim.value.single);
                       // if( at < (int)it->second.size()-1 )
                           // fprintf(fp, " ");
                    }
                }
                //fprintf(fp, "\n");
            }
        }
    }
    */
    
    fclose(fp);
}


bool TColladaToGASExporter::ExportMesh(TColladaModel* model, const char* path)
{
    
    FILE* fp = fopen(path, "wt");
    assert( fp );
    
    
    TColladaVisualScene* visualScene = model->visualScenes[0];
    
    for(int j=0;j<visualScene->rootNodes.size();++j)
    {
        TColladaNode* node = visualScene->rootNodes[j];
        if( node->IsRootNode())
            node->UpdateTransform();
    }
    
    std::map<int, TColladaNode*> jointMap;
    std::map<TColladaNode*, int> jointReverseMap;
    
    std::stack<TColladaNode*> st;
    
    std::map<TColladaNode*, Matrix4> inverseBindPoseMap;
    
    //int treeCount = 0;
    
    
    int nodeId = 0;
    
    /*
    for(int j=0;j<visualScene->jointNodes.size();++j)
    {
        TColladaNode* node = visualScene->jointNodes[j];
        std::string name = node->name;
        
        Matrix4 invBindPose;
        
        bool found = model->GetInverseBindPoseByJointName(name, invBindPose);
        if( found )
        {
            jointMap[nodeId] = node;
            jointReverseMap[node] = nodeId;
            ++ nodeId;
        }
    }
     */
    
    // re-arrange the joints so that parent go first in the listing
    int treeCount = 0;
    
    std::map<std::string, std::string> jointSIDToIDMap;
    for(int j=0;j<visualScene->allNodes.size();++j)
    {
        TColladaNode* node = visualScene->allNodes[j];
        int nodeId = 0;
        if( node->IsRootNode())
        {
            ++ treeCount;
            st.push(node);
            while(!st.empty())
            {
                TColladaNode* top = st.top();
                st.pop();
                
                std::string& name = top->sid;
                Matrix4 invBindPose;
                bool found = model->GetInverseBindPoseByJointName(name, invBindPose);
                if( found )
                {
                    jointMap[nodeId] = top;
                    jointReverseMap[top] = nodeId;
                    
                    jointSIDToIDMap[top->sid] = top->id;
                    
                    inverseBindPoseMap[top] = invBindPose;
                    
                    ++ nodeId;
                }
                    for(int child=(int)top->childs.size()-1;child>=0;--child)
                    {
                        st.push(top->childs[child]);
                    }
                
            }
        }
    }
    
   // assert( treeCount == 1 );
    
    // write the joint data
    int jointCount = (int)jointMap.size();
    
    int meshCount = (int)model->controllers.size();
    
    fprintf(fp, "GAS Version 01\n");
    
 
 
    fprintf(fp, "numJoints %d\n", jointCount);
    fprintf(fp, "numMeshes %d\n", meshCount);
    
    
    fprintf(fp, "joints {\n");
    
    std::map<int, TColladaNode*>::iterator it = jointMap.begin();
    for(;it!=jointMap.end();++it)
    {
        TColladaNode* joint = it->second;
        std::string name = joint->sid;
        
        int parentId = -1;
        if( joint->parentNode )
        {
            std::map<TColladaNode*, int>::iterator rit = jointReverseMap.find(joint->parentNode);
            if( rit != jointReverseMap.end())
            {
                parentId = jointReverseMap[joint->parentNode];
            }
        }
        
        
        Matrix4 transform = joint->baseTransform;
        
       
        
        Vector3 p = transform.getTrans();
        
        
        Matrix3 mat3;
        transform.extract3x3Matrix(mat3);
        Quaternion q;
        q.FromRotationMatrix(mat3);
        
        
        
        Matrix4 invBindPose = Matrix4::IDENTITY.inverse();
        
        bool found = model->GetInverseBindPoseByJointName(name, invBindPose);
        assert( found );
        
        //invBindPose = invBindPose.inverse();
        
        
        Vector3 ip = invBindPose.getTrans();
        Matrix3 rotation;
        invBindPose.extract3x3Matrix(rotation);
        Quaternion iq;
        iq.FromRotationMatrix(rotation);
        iq.normalise();
        
        //int nameLength = (int)strlen(name.c_str());
        fprintf(fp, "\t \"%s\" %d ( %f %f %f ) ( %f %f %f ) ( %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f )\n", name.c_str(), parentId, p.x, p.y, p.z, q.x ,q.y, q.z,
                invBindPose._m[0], invBindPose._m[1], invBindPose._m[2], invBindPose._m[3],
                invBindPose._m[4], invBindPose._m[5], invBindPose._m[6], invBindPose._m[7],
                invBindPose._m[8], invBindPose._m[9], invBindPose._m[10], invBindPose._m[11],
                invBindPose._m[12], invBindPose._m[13], invBindPose._m[14], invBindPose._m[15]
                );
        
    }
    
    
    fprintf(fp, "}\n");
    
    for(int con=0;con<(int)model->controllers.size();++con)
    {
        
        fprintf(fp, "mesh {\n");
        TColladaSkin* skin = model->controllers[con]->skin;
        TColladaGeometry* geometry = NULL;
        if( skin )
        {
            std::string gid = TColladaParserUtils::SkipStartChar(skin->source);
            geometry = model->FindGeometryById(gid);
        }
        
        if( ! geometry )
            continue;
        
        TColladaVertexWeights* vertexWeights = NULL;
        TColladaSource* weightSource = NULL;
        TColladaSource* jointNameSource = NULL;
        if( skin )
        {
            vertexWeights = skin->vertexWeights;
            weightSource = skin->GetWeightSource();
            jointNameSource = skin->GetJointNameSource();
    
        
            assert( vertexWeights );
            assert( weightSource );
            assert( jointNameSource );
        
        }
        
    
        
        
        TColladaMesh* mesh = geometry->mesh;
        
        bool can = CanBakeUVToVertexWithoutUsingNewVertex(mesh);
        
        
        int polyListCount = (int)mesh->polylists.size();
        
        std::vector<TGBOTriangle> triangles;
        std::vector<TDataItem> itemItems;
        TColladaSource* vertexSource = NULL;
        
        for(int j=0;j<mesh->polylists.size();++j)
        {
            TColladaPolyList* list = mesh->polylists[j];
            list->IsValid();
            // skip the #
            size_t length = list->vertexSource.length();
            std::string s = list->vertexSource.substr(1, length-1);
            
            vertexSource = mesh->FindVertexSourceById(s);
            if( vertexWeights && vertexSource )
            {
                assert( 3 * vertexWeights->vcount.size() == vertexSource->values.size() );
            }
            
            assert( vertexSource );
            length = list->texCoordSource.length();
            s = list->texCoordSource.substr(1, length-1);
            TColladaSource* textureSource = mesh->FindSourceById(s);
        
            int vCount = (int)vertexSource->values.size() / (int)vertexSource->stride;
            for(int j=0;j<vCount;++j)
            {
                float x = vertexSource->values[3*j+0];
                float y = vertexSource->values[3*j+1];
                float z = vertexSource->values[3*j+2];
                TDataItem item;
                item.vertex = j;
                item.position = Vector3(x, y, z);
                item.u = 0.0f;
                item.v = 0.0f;
                item.weightStart = -1;
                item.weightCount = -1;
                itemItems.push_back(item);
            }
            int offset = 0;
            int startIndex = offset;
            for(int v=0;v<list->vcount.size();++v)
            {
                int vertexCount = list->vcount[v];
                assert( vertexCount>=3 );
                
                // divide the polygon to triangles
                for(int k=2;k<vertexCount;++k)
                {
                    int stride = 3;
                    int vv[3] = { 0, k - 1, k };
                    
                    int vertexID[3];
                    for(int vi=0;vi<3;++vi)
                    {
                        int i = (vv[vi]) * stride + startIndex;
                        vertexID[vi] = list->p[i];
                        //int ii = vertexID[vi] * 3; // x, y, z
                        float u0 = 0.0f, v0 = 0.0f;
                        if( textureSource )
                        {
                            int t0 = list->textureOffset + (vv[vi]) * stride + startIndex;
                            int uu0 = list->p[t0] * 2; // 2 = u + v
                            u0 = textureSource->values[uu0];
                            v0 = textureSource->values[uu0+1];
                        }
                        itemItems[vertexID[vi]].u = u0;
                        itemItems[vertexID[vi]].v = v0;
                    }
                    
                    TGBOTriangle triangle(vertexID[0], vertexID[1], vertexID[2]);
                    triangles.push_back(triangle);
                }
                startIndex += vertexCount * list->stride;
            }
        }
        for(int j=0;j<mesh->triangles.size();++j)
        {
            TColladaTriangle* triangleLists = mesh->triangles[j];
            // skip the #
            size_t length = triangleLists->vertexSource.length();
            std::string s = triangleLists->vertexSource.substr(1, length-1);
            
            vertexSource = mesh->FindVertexSourceById(s);
            if( vertexWeights && vertexSource )
            {
                assert( 3 * vertexWeights->vcount.size() == vertexSource->values.size() );
            }
            
            assert( vertexSource );
            length = mesh->texCoordSource.length();
            s = mesh->texCoordSource.substr(1, length-1);
            TColladaSource* textureSource = mesh->FindSourceById(s);
            
            int vCount = (int)vertexSource->values.size() / (int)vertexSource->stride;
            for(int j=0;j<vCount;++j)
            {
                float x = vertexSource->values[3*j+0];
                float y = vertexSource->values[3*j+1];
                float z = vertexSource->values[3*j+2];
                TDataItem item;
                item.vertex = j;
                item.position = Vector3(x, y, z);
                item.u = 0.0f;
                item.v = 0.0f;
                item.weightStart = -1;
                item.weightCount = -1;
                itemItems.push_back(item);
            }
            
            assert( triangleLists->p.size() % 3 == 0 );
            int triangleCount = (int)triangleLists->p.size() / 3;
            int startIndex = 0;
            for(int v=0;v<triangleCount;++v)
            {
                int vertexID[3];
                int stride = 3;
                
                for(int vi=0;vi<3;++vi)
                {
                    int i = startIndex;
                    vertexID[vi] = triangleLists->p[i];
                    //int ii = vertexID[vi] * 3; // x, y, z
                    float u0 = 0.0f, v0 = 0.0f;
                    if( textureSource )
                    {
                        int textureOffset = 2;
                        int t0 = textureOffset + vi * stride + startIndex;
                        int uu0 = triangleLists->p[t0] * 2; // 2 = u + v
                        u0 = textureSource->values[uu0];
                        v0 = textureSource->values[uu0+1];
                    }
                    itemItems[vertexID[vi]].u = u0;
                    itemItems[vertexID[vi]].v = v0;
                    startIndex ++;
                }
                
                TGBOTriangle triangle(vertexID[0], vertexID[1], vertexID[2]);
                triangles.push_back(triangle);
            }
        }

        int weightStart = 0;
        for(int j=0;j<(int)vertexWeights->vcount.size();++j)
        {
            int weightCount = vertexWeights->vcount[j];
            itemItems[j].weightStart = weightStart;
            itemItems[j].weightCount = weightCount;
            
            for(int w=0;w<weightCount;++w)
            {
                TGBOWeight weight;
            }
            weightStart += weightCount;
        }
       
        // write the vertices
        int vCount = (int)itemItems.size();
        fprintf(fp, "numverts %d\n", vCount);
        for(int j=0;j<itemItems.size();++j)
        {
            TDataItem item = itemItems[j];
            float x = item.position.x;
            float y = item.position.y;
            float z = item.position.z;
            float u = item.u;
            float v = item.v;
            int weightStart = item.weightStart;
            int weightCount = item.weightCount;
            
            
            // bake the bind shape matrix to the vertex
            if( skin )
            {
                Vector4 p(x, y, z, 1);
                p = skin->bindShapeMatrix * p;
                x = p.x;
                y = p.y;
                z = p.z;
            }
           
            fprintf(fp, "\t vert %d ( %f %f %f ) ( %f %f ) %d %d\n", j, x, y, z, u, v, weightStart, weightCount);
        }
        
        // write the triangles
        int triangleCount = (int)triangles.size();
        fprintf(fp,"numtris %d\n", triangleCount);
        for(int j=0;j<(int)triangles.size();++j)
        {
            assert( triangles[j].v0 < (int)itemItems.size());
            assert( triangles[j].v1 < (int)itemItems.size());
            assert( triangles[j].v2 < (int)itemItems.size());
            fprintf(fp, "\t tri %d %d %d %d\n", j, triangles[j].v0, triangles[j].v1, triangles[j].v2);
        }
        
        
        // weights
        if( weightSource )
        {            
            int weightCount = 0;
            for(int vc=0;vc<(int)vertexWeights->vcount.size();++vc)
                weightCount += vertexWeights->vcount[vc];
            
            int wid = 0;
            // write the weights
            fprintf(fp, "numweights %d\n", weightCount);
            for(int vc=0;vc<(int)vertexWeights->vcount.size();++vc)
            {
                int vertexWeightCount = vertexWeights->vcount[vc];
                if( vertexWeightCount == 0 )
                    continue;
                
                assert( vc<(int)vertexWeights->startJointIndices.size());
                int jointOffset = vertexWeights->startJointIndices[vc];
                
                float sum = 0.0f;
                for(int v=0;v<vertexWeightCount;++v)
                {
                    int jointIndex = vertexWeights->v[jointOffset];
                    int weightOffset = vertexWeights->v[jointOffset + 1];
                    
                    
                    std::string jointName = jointNameSource->nameArray->names[jointIndex];
                    assert( jointIndex<jointNameSource->nameArray->names.size() );
                    assert( jointIndex < (int)skin->invBindPoses.size());
                    
                    TColladaNode* joint = visualScene->GetJointNodeBySID(jointName);
                    
                    std::map<TColladaNode*, int>::iterator it = jointReverseMap.find(joint);
                    assert( it!=jointReverseMap.end());
                    
                    int jjjIndex = jointReverseMap[joint];
                    assert( jjjIndex>=0 );
                    
                    float weight = weightSource->values[weightOffset];
                    
                    TGBOWeight w(jjjIndex, weight);
                    
                    fprintf(fp, "\t weight %d %d %f \n", wid, jjjIndex, weight);
                    jointOffset += 2;
                    sum += weight;
                    
                    weightCount --;
                    
                    ++wid;
                }
                assert( sum>=0.9f && sum < 1.1f );
            }
            fprintf(fp, "}\n");
            assert( weightCount == 0 );
        }
    }
    
     
    fclose(fp);

    
    return true;
}


void TColladaToGASExporter::ExportGBO(TColladaModel* model, const char* filename)
{
    FILE* fp = fopen(filename, "w");
    assert( fp );
    
    
    TColladaVisualScene* visualScene = model->visualScenes[0];
    
    for(int j=0;j<visualScene->rootNodes.size();++j)
    {
        TColladaNode* node = visualScene->rootNodes[j];
        assert( node->IsRootNode());
        node->UpdateTransform();
    }
    
    std::map<int, TColladaNode*> jointMap;
    std::map<TColladaNode*, int> jointReverseMap;
    
    std::stack<TColladaNode*> st;
    
    //int treeCount = 0;
    int nodeId = 0;
    for(int j=0;j<visualScene->jointNodes.size();++j)
    {
        TColladaNode* node = visualScene->jointNodes[j];
        jointMap[nodeId] = node;
        jointReverseMap[node] = nodeId;
        ++ nodeId;
    }
    
    /*
    for(int j=0;j<visualScene->jointNodes.size();++j)
    {
        TColladaNode* node = visualScene->jointNodes[j];
        int nodeId = 0;
        if( node->IsRootNode())
        {
            ++ treeCount;
            st.push(node);
            while(!st.empty())
            {
                TColladaNode* top = st.top();
                st.pop();
                jointMap[nodeId] = top;
                jointReverseMap[top] = nodeId;
                
                
                ++ nodeId;
                for(int child=(int)top->childs.size()-1;child>=0;--child)
                {
                    st.push(top->childs[child]);
                }
            }
        }
    }
    
    assert( treeCount == 1 );
     */
   

    
    TGBOHeader header;
    header.magic[0] = 'G';
    header.magic[1] = 'B';
    header.magic[2] = 'O';
    header.magic[3] = 0;
    
    header.version = 1;
    
    fwrite(&header, sizeof(header), 1 , fp);
    
    
    // write the joint data
    int jointCount = (int)jointMap.size();
    
    
    
    
   
    fwrite(&jointCount, sizeof(int), 1, fp);
    
    std::map<int, TColladaNode*>::iterator it = jointMap.begin();
    for(;it!=jointMap.end();++it)
    {
        TColladaNode* joint = it->second;
        std::string name = joint->name;
        
        int parentId = -1;
        if( joint->parentNode )
        {
            std::map<TColladaNode*, int>::iterator rit = jointReverseMap.find(joint->parentNode);
            if( rit != jointReverseMap.end())
            {
                parentId = jointReverseMap[joint->parentNode];
            }
        }
        
        int nameLength = (int)strlen(name.c_str());
        fwrite(&nameLength, sizeof(int), 1, fp);
        fwrite(name.c_str(), nameLength, 1, fp);
        fwrite(&parentId, sizeof(int),1 , fp);
        
        Matrix4 mat = joint->baseTransform;
        
        Vector3 p = mat.getTrans();
        
        
        Matrix3 mat3;
        mat.extract3x3Matrix(mat3);
        Quaternion q;
        q.FromRotationMatrix(mat3);
        
        fwrite(&p.x, sizeof(float), 1, fp);
        fwrite(&p.y, sizeof(float), 1, fp);
        fwrite(&p.z, sizeof(float), 1, fp);
        fwrite(&q.x, sizeof(float), 1, fp);
        fwrite(&q.y, sizeof(float), 1, fp);
        fwrite(&q.z, sizeof(float), 1, fp);
        
        
        
        // write the inverse bind pose of the joint
        Matrix4 invBindPose;
        bool found = model->GetInverseBindPoseByJointName(name, invBindPose);
        
        Vector3 ip = invBindPose.getTrans();
        Matrix3 rotation;
        invBindPose.extract3x3Matrix(rotation);
        Quaternion iq;
        iq.FromRotationMatrix(rotation);
        fwrite(&ip.x, sizeof(float), 1, fp);
        fwrite(&ip.y, sizeof(float), 1, fp);
        fwrite(&ip.z, sizeof(float), 1, fp);
        fwrite(&iq.x, sizeof(float), 1, fp);
        fwrite(&iq.y, sizeof(float), 1, fp);
        fwrite(&iq.z, sizeof(float), 1, fp);
                
        
    }
    
    
    
    int meshCount = (int)model->controllers.size();
    fwrite(&meshCount, sizeof(int), 1, fp);
    
    for(int con=0;con<(int)model->controllers.size();++con)
    {
        TColladaSkin* skin = model->controllers[con]->skin;
        TColladaGeometry* geometry = NULL;
        if( skin )
        {
            std::string gid = TColladaParserUtils::SkipStartChar(skin->source);
            geometry = model->FindGeometryById(gid);
        }
        
        if( ! geometry )
            continue;
        
        TColladaVertexWeights* vertexWeights = NULL;
        TColladaSource* weightSource = NULL;
        TColladaSource* jointNameSource = NULL;
        if( skin )
        {
            vertexWeights = skin->vertexWeights;
            weightSource = skin->GetWeightSource();
            jointNameSource = skin->GetJointNameSource();
        }
        
        
        TColladaMesh* mesh = geometry->mesh;
        int polyListCount = (int)mesh->polylists.size();
        
        std::map<int, TDataItem> dataItems;
        std::vector<TGBOTriangle> triangles;
        
        
        for(int j=0;j<mesh->polylists.size();++j)
        {
            TColladaPolyList* list = mesh->polylists[j];
            list->IsValid();
            
            
            // skip the #
            size_t length = list->vertexSource.length();
            std::string s = list->vertexSource.substr(1, length-1);
            
            TColladaSource* vs = mesh->FindVertexSourceById(s);
            
            assert( vs );
            length = list->texCoordSource.length();
            s = list->texCoordSource.substr(1, length-1);
            TColladaSource* textureSource = mesh->FindSourceById(s);
            
            int offset = 0;
            int startIndex = offset;
            
            for(int v=0;v<list->vcount.size();++v)
            {
                int vertexCount = list->vcount[v];
                assert( vertexCount>=3 );
                
                TFlatRenderer::TVertexData data[3];
                
                // divide the polygon to triangles
                for(int k=2;k<vertexCount;++k)
                {
                    int stride = 3;
                    
                    int vv[3];
                    vv[0] = 0;
                    vv[1] = k - 1;
                    vv[2] = k;
                    
                    int vertexID[3];
                    for(int vi=0;vi<3;++vi)
                    {
                        int i = (vv[vi]) * stride + startIndex;
                        
                        vertexID[vi] = list->p[i];
                        
                        int ii = vertexID[vi] * 3; // x, y, z
                        Vector3 p0;
                        
                        p0.x = vs->values[ii];
                        p0.y = vs->values[ii+1];
                        p0.z = vs->values[ii+2];
                        
                        float u0 = 0.0f;
                        float v0 = 0.0f;
                        
                        if( textureSource )
                        {
                            int t0 = list->textureOffset + (vv[vi]) * stride + startIndex;
                            int uu0 = list->p[t0] * 2; // 2 = u + v
                            u0 = textureSource->values[uu0];
                            v0 = textureSource->values[uu0+1];
                        }
                        
                        Vector4 p = Vector4(p0.x, p0.y, p0.z, 1.0f);
                        
                        if( skin )
                        {
                            p = Vector4::ZERO;
                            int weightCount = vertexWeights->vcount[vertexID[vi]];
                            int jointOffset = vertexWeights->startJointIndices[vertexID[vi]];
                            
                            float sum = 0.0f;
                            int jointCount = (int)visualScene->jointNodes.size();
                            
                            
                            int weightOffset = vertexWeights->v[jointOffset + 1];
                            
                            std::map<int, TDataItem>::iterator it = dataItems.find(vertexID[vi]);
                            if( it == dataItems.end())
                            {
                                TDataItem item;
                                item.vertex = vertexID[vi];
                                item.position = p0;
                                item.weightStart = weightOffset;
                                item.weightCount = weightCount;
                                dataItems[vertexID[vi]] = item;
                            }
                            else
                            {
                                TDataItem item = it->second;
                                assert( item.weightStart == weightOffset);
                                assert( item.weightCount == weightCount);
                                assert( item.vertex == vertexID[vi]);
                            }
                            
                            
                            for(int w=0;w<weightCount;++w)
                            {
                                int jointIndex = vertexWeights->v[jointOffset];
                                weightOffset = vertexWeights->v[jointOffset + 1];
                                
                                assert( jointIndex<jointCount );
                                
                                Matrix4 inverseBindPose = skin->invBindPoses[jointIndex];
                                
                                std::string jointName = jointNameSource->nameArray->names[jointIndex];
                                
                                TColladaNode* joint = visualScene->GetJointNodeBySID(jointName);
                                
                                
                                
                                float weight = weightSource->values[weightOffset];
                                
                                assert( weight>=0.0f && weight<=1.0f);
                                
                                Vector4 vertex(p0.x, p0.y, p0.z, 1.0f);
                                p += joint->worldTransform * inverseBindPose * vertex * weight;
                                
                                sum += weight;
                                
                                jointOffset += 2;
                            }
                            
                            assert( sum>=0.9f && sum<=1.01f );
                            
                            //if( sum != 1.0f && sum!=0.0f )
                            //    p /= sum;
                        }
                        p0 = Vector3(p.x, p.y, p.z);
                        
                        
                        
                        
                        //float s = 20.0f;
                        // p0 = p0*s;
                        
                        data[vi] = TFlatRenderer::TVertexData(p0.x, p0.y, p0.z, u0, v0);
                    }
                    
                    TGBOTriangle triangle;
                    triangle.v0 = vertexID[0];
                    triangle.v1 = vertexID[1];
                    triangle.v2 = vertexID[2];
                    triangles.push_back(triangle);
                }
                
                startIndex += vertexCount * list->stride; // vertex, normal , texture
                
                
            }
        }
        
        size_t ff = sizeof(float);
        int vCount = (int)dataItems.size();
        fwrite(&vCount, sizeof(int), 1, fp);
        for(int j=0;j<(int)dataItems.size();++j)
        {
            float x = dataItems[j].position.x;
            float y = dataItems[j].position.y;
            float z = dataItems[j].position.z;
            float u = 0.0f;
            float v = 0.0f;
            int weightStart = dataItems[j].weightStart;
            int weightCount = dataItems[j].weightCount;
            
            //fwrite(&x, sizeof(x), 1, fp);
            //fwrite(&y, sizeof(y), 1, fp);
            //fwrite(&z, sizeof(z), 1, fp);
            //fwrite(&u, sizeof(u), 1, fp);
            //fwrite(&v, sizeof(v), 1, fp);
            //fwrite(&weightStart, sizeof(weightStart), 1, fp);
            //fwrite(&weightCount, sizeof(weightCount), 1, fp);
            //int dummy = 0;
            //fwrite(&dummy, sizeof(dummy), 1, fp);
        }

        int triangleCount = (int)triangles.size();
        int written = fwrite(&triangleCount, sizeof(triangleCount), 1, fp);
        assert( written == 1);
        fflush(fp);

        //fprintf(fp, "\n");
        //fprintf(fp, "numtris %d\n", triangleCount);
        
        int iid = 0;
        for(int j=0;j<(int)triangles.size();++j)
        {
            int vertexIDs[3];
            vertexIDs[0] = triangles[j].v0;
            vertexIDs[1] = triangles[j].v1;
            vertexIDs[2] = triangles[j].v2;
            
            
            fwrite(&vertexIDs[0], sizeof(int), 1, fp);
            fwrite(&vertexIDs[1], sizeof(int), 1, fp);
            fwrite(&vertexIDs[2], sizeof(int), 1, fp);
            
            // fprintf(fp, "\t tri %d %d %d %d\n", j, vertexIDs[0], vertexIDs[1], vertexIDs[2]);
            iid += 3;
        }


        
        
        for(int j=0;j<mesh->polylists.size();++j)
        {
            TColladaPolyList* polylist = mesh->polylists[j];
            assert( polylist->IsValid() );
            // skip the #
            size_t length = polylist->vertexSource.length();
            std::string s = polylist->vertexSource.substr(1, length-1);
            
            TColladaSource* vertexSource = mesh->FindVertexSourceById(s);
            assert( vertexSource );
            
            assert( vertexSource->stride > 0  );
            
            length = polylist->texCoordSource.length();
            s = polylist->texCoordSource.substr(1, length-1);
            TColladaSource* textureSource = mesh->FindSourceById(s);
            
            int vertexCount = (int)vertexSource->values.size() / vertexSource->stride;
            
            
            // save the position first
            std::vector<Vector3> positions;
            int vIndex = 0;
            for(int j=0;j<vertexCount;++j)
            {
                float x = vertexSource->values[vIndex+0];
                float y = vertexSource->values[vIndex+1];
                float z = vertexSource->values[vIndex+2];
                positions.push_back(Vector3(x,y,z));
                vIndex += vertexSource->stride;
            }
            
            // mesh
            assert( vertexCount == (int)vertexWeights->vcount.size());
            
            std::vector<TVertexItem> items;
            int ssss = 0;
            
            std::vector<int> startWeights;
            std::vector<int> weightCounts;
            
            startWeights.resize(vertexCount);
            weightCounts.resize(vertexCount);
            
            
            
 
            
            TColladaSource* weightSource = skin->GetWeightSource();
            
            
            int weightCount = 0;
            for(int vc=0;vc<(int)vertexWeights->vcount.size();++vc)
            {
                weightCount += vertexWeights->vcount[vc];
            }

            
            
            //fprintf(fp, "\n");
            //fprintf(fp, "numweights %d\n", weightCount);
            
            //fwrite(&weightCount, sizeof(int), 1, fp);
            
            
            std::map<TColladaNode*, int> jointIndexMap;
            std::map<TColladaNode*, int> jointWeightMap;
            
            int wid = 0;
            //assert( vertexSource->values.size() == 3 * vertexWeights->vcount.size());
            
            
            
            #if 0
            for(int vc=0;vc<(int)vertexWeights->vcount.size();++vc)
            {
                //sum += vertexWeights->vcount[vc];
                int vertexWeightCount = vertexWeights->vcount[vc];
                
                assert( vc<(int)vertexWeights->startJointIndices.size());
                int jointOffset = vertexWeights->startJointIndices[vc];
                assert( vc*3+2 < (int) vertexSource->values.size());
                
                float sum = 0.0f;
                for(int v=0;v<vertexWeightCount;++v)
                {
                    int jointIndex = vertexWeights->v[jointOffset];
                    int weightOffset = vertexWeights->v[jointOffset + 1];
                    
                    assert( jointIndex<jointCount );
                    assert( jointIndex < (int)skin->invBindPoses.size());
                    
                    std::string jointName = jointNameSource->nameArray->names[jointIndex];
                    
                    TColladaNode* joint = visualScene->GetJointNodeBySID(jointName);
                    
                    std::map<TColladaNode*, int>::iterator it = jointReverseMap.find(joint);
                    assert( it!=jointReverseMap.end());

                    //int jjjIndex = jointReverseMap[joint];
                    std::map<int , TColladaNode*>::iterator iit = jointMap.begin();
                    int jjjIndex = -1;
                    int kk = 0;
                    for(;iit!=jointMap.end();++iit)
                    {
                        if( iit->second->name == jointName )
                        {
                            jjjIndex = kk;
                            break;
                        }
                        ++ kk;
                    }
                    
                    //jjjIndex = jointIndex;
                    assert( jjjIndex>=0);
                    
                    float weight = weightSource->values[weightOffset];
                    
                    
                   // fwrite(&jjjIndex, sizeof(int), 1, fp);
                   // fwrite(&weight, sizeof(float), 1, fp);
                    weightCount --;
                   // fprintf(fp, "\t weight %d %d %f \n", wid, jjjIndex, weight);
                    jointOffset += 2;
                    sum += weight;
                    ++wid;
                }
                assert( sum>=0.9f && sum < 1.1f );
                // offset += vertexWeights->vcount[vc];
            }
           // fprintf(fp, "\n");
           // fprintf(fp, "}\n");
        
        assert( weightCount == 0 );

        #endif

        
        }
   
    }
    
    fflush(fp);
    
    fclose(fp);
}

int TColladaToGASExporter::UpdateVertexItem(std::vector<TVertexItem>& vertexItems, TVertexItem& item) const
{
    assert( item.vertex>=0 && item.uv>=0 );
    assert( item.vertex < (int)vertexItems.size() );
    if( vertexItems[item.vertex].uv==-1 )
    {
        vertexItems[item.vertex].x = item.x;
        vertexItems[item.vertex].y = item.y;
        vertexItems[item.vertex].z = item.z;
        vertexItems[item.vertex].weightStart = item.weightStart;
        vertexItems[item.vertex].weightCount = item.weightCount;
        vertexItems[item.vertex].u = item.u;
        vertexItems[item.vertex].v = item.v;
        vertexItems[item.vertex].vertex = item.vertex;
        vertexItems[item.vertex].uv = item.uv;
        return item.vertex;
    }
    else if( vertexItems[item.vertex].vertex==item.vertex && vertexItems[item.vertex].uv==item.uv )
    {
        return item.vertex;
    }
    
    TVertexItem newItem = item;
    newItem.vertex = (int)vertexItems.size();
    vertexItems.push_back(newItem);
    return (int)vertexItems.size() - 1;
}


void TColladaToGASExporter::ExportGMO(TColladaModel* model, const char* path)
{
    FILE* fp = fopen(path, "wb");
    assert( fp  );
    
    TGMOHeader header;
    header.magic[0] = 'G';
    header.magic[1] = 'M';
    header.magic[2] = 'O';
    header.magic[3] = '\0';
    
    header.version = 1;
    fwrite(&header, sizeof(header), 1, fp);
    
       
    std::vector<TColladaMesh*> meshes;
    
    
    std::vector<TColladaNode*> nodes;
    std::vector<TColladaInstanceGeometry*> instanceGeometries;
    
    
    int meshCount = 0;
    for(int s=0;s<(int)model->visualScenes.size();++s)
    {
        //int nodeCount = (int)model->visualScenes[s]->allNodes.size();
        std::map<std::string, std::string> instanceMap;
        TColladaVisualScene* visualScene = model->visualScenes[s];
        for(int n=0;n<(int)visualScene->allNodes.size();++n)
        {
            TColladaNode* node = visualScene->allNodes[n];
            
            nodes.push_back(node);
            
            
            
            for(int ig=0;ig<(int)node->instanceGeometrys.size();++ig)
            {
                TColladaInstanceGeometry* instanceGeometry = node->instanceGeometrys[ig];
                
                instanceGeometries.push_back(instanceGeometry);
                
                TColladaNode* node = instanceGeometry->parentNode;
                assert( node );
                
                TColladaGeometry* geometry = model->FindGeometryById(instanceGeometry->url.substr(1));
                assert( geometry );
                if( geometry->mesh )
                {
                    meshes.push_back(geometry->mesh);
                    ++ meshCount;
                }
            }
        }
    }
    
    // write the nodes
    int nodeCount = (int)nodes.size();
    fwrite(&nodeCount, sizeof(int), 1, fp);
    for(int j=0;j<nodeCount;++j)
    {
        TColladaNode* node = nodes[j];
        int nameLength = (int)node->name.length();
        fwrite(&nameLength, sizeof(int), 1, fp);
        fwrite(node->name.c_str(), nameLength, 1, fp);
        int parentID = -1;
        for(int p=0;p<nodeCount;++p)
        {
            if( nodes[p] == node->parentNode )
            {
                parentID = p;
                break;
            }
        }
       
    
        fwrite(&parentID, sizeof(int), 1, fp);
    
        Matrix4 transform = node->baseTransform;
    
        fwrite(transform._m, sizeof(float), 16, fp);
    }
    
    int instanceCount = (int)instanceGeometries.size();
    fwrite(&instanceCount, sizeof(int), 1, fp);
    for(int k=0;k<instanceCount;++k)
    {
        TColladaInstanceGeometry* g = instanceGeometries[k];
        TColladaNode* parentNode = g->parentNode;
        int nodeIDLength = (int)parentNode->id.length();
        fwrite(&nodeIDLength, sizeof(int), 1, fp);
        fwrite(parentNode->id.c_str(), nodeIDLength, 1, fp);
        
        std::string url = g->url;
        url = url.substr(1);
        int geometryURLLength = (int)url.length();
        fwrite(&geometryURLLength, sizeof(int), 1, fp);
        fwrite(url.c_str(), geometryURLLength, 1, fp);
    }
    
    fwrite(&meshCount, sizeof(int), 1, fp);
    for(int m=0;m<meshes.size();++m)
    {
        TColladaMesh* mesh = meshes[m];
        
        
        
        
        
        
        for(int plist=0;plist<mesh->polylists.size();++plist)
        {
            TColladaPolyList* polylist = mesh->polylists[plist];
            if( ! polylist )
                continue;
            
            
            size_t length = polylist->vertexSource.length();
            std::string s = polylist->vertexSource.substr(1, length-1);
            
            TColladaSource* vertexSource = mesh->FindVertexSourceById(s);
            
            std::vector<TVertexItem> vertexItems;
            std::vector<TGMOTriangle> triangles;
            
            int vertexCount = vertexSource->values.size() / 3;
            
            
            
           
            
            vertexItems.resize(vertexCount);
            
            
            assert( vertexSource );
            length = polylist->texCoordSource.length();
            s = polylist->texCoordSource.substr(1, length-1);
            TColladaSource* textureSource = mesh->FindSourceById(s);
            
            
            int startIndex = 0;
            for(int v=0;v<polylist->vcount.size();++v)
            {
                int vertexCount = polylist->vcount[v];
                for(int k=2;k<vertexCount;++k)
                {
                    int stride = 3;
                    
                    int vv[3];
                    vv[0] = 0;
                    vv[1] = k - 1;
                    vv[2] = k;
                    
                    int triIndex[3];
                    int uvIndex[3];
                    
                    
                    Vector3 position[3];
                    Vector2 uvs[3];
                    
                    for(int vi=0;vi<3;++vi)
                    {
                        int i = (vv[vi]) * stride + startIndex;
                        int vertexID = polylist->p[i];
                        triIndex[vi] = vertexID;
                        int ii = vertexID * 3; // x, y, z
                        position[vi].x = vertexSource->values[ii + 0];
                        position[vi].y = vertexSource->values[ii + 1];
                        position[vi].z = vertexSource->values[ii + 2];
                        
                        int t0 = polylist->textureOffset + (vv[vi]) * stride + startIndex;
                        uvIndex[vi] = polylist->p[t0] * 2; // 2 = u + v
                        uvs[vi].x = textureSource->values[uvIndex[vi]];
                        uvs[vi].y = textureSource->values[uvIndex[vi]+1];
                    }
                    
                    TVertexItem items[3];
                    for(int it=0;it<3;++it)
                    {
                        items[it].vertex = triIndex[it];
                        items[it].uv = uvIndex[it];
                        items[it].x = position[it].x;
                        items[it].y = position[it].y;
                        items[it].z = position[it].z;
                        items[it].u = uvs[it].x;
                        items[it].v = uvs[it].y;
                    }
                    
                    int id0 = UpdateVertexItem(vertexItems, items[0]);
                    int id1 = UpdateVertexItem(vertexItems, items[1]);
                    int id2 = UpdateVertexItem(vertexItems, items[2]);
                    
                    assert( id0 >= 0 );
                    assert( id1 >= 0 );
                    assert( id2 >= 0 );
                    
                    //TGASTriangle triangle(vertexItems[id0].vertex, vertexItems[id1].vertex, vertexItems[id2].vertex);
                    
                    TGMOTriangle triangle(id0, id1, id2);
                    triangles.push_back(triangle);
                }
                startIndex += vertexCount * polylist->stride; // vertex, normal , texture
            }
            
            
            assert( mesh->geometry );
            const std::string& name = mesh->geometry->id;
            
            int nameLength = (int)name.length();
            
            fwrite(&nameLength, sizeof(int), 1, fp);
            fwrite(name.c_str(), nameLength, 1, fp);
            
            
            int vCount = (int)vertexItems.size();
            fwrite(&vCount, sizeof(int), 1, fp);
            for(int vv=0;vv<(int)vertexItems.size();++vv)
            {
                float x = vertexItems[vv].x;
                float y = vertexItems[vv].y;
                float z = vertexItems[vv].z;
                float u = vertexItems[vv].u;
                float v = vertexItems[vv].v;
                fwrite(&x, sizeof(float), 1, fp);
                fwrite(&y, sizeof(float), 1, fp);
                fwrite(&z, sizeof(float), 1, fp);
                fwrite(&u, sizeof(float), 1, fp);
                fwrite(&v, sizeof(float), 1, fp);
            }
            
            int triangleCount = (int)triangles.size();
            fwrite(&triangleCount, sizeof(int), 1, fp);
            for(int t=0;t<triangleCount;++t)
            {
                fwrite(&triangles[t].v0, sizeof(int), 1, fp);
                fwrite(&triangles[t].v1, sizeof(int), 1, fp);
                fwrite(&triangles[t].v2, sizeof(int), 1, fp);
                
            }
            int b = 0;
        }
        
        
        
        int a = 0;
        
    }
    
    
    
    std::vector<TColladaAnimation*> animations;
    model->CollectAnimations(animations);
    
    
    std::map<std::string, std::vector<TColladaAnimation*>> animationMap;
    for(int anim=0;anim<(int)animations.size();++anim)
    {
        TColladaAnimation* animation = animations[anim];
        if( ! animation->GetTimeSource() )
            continue;
        
        std::string target = animation->target;
        size_t p = target.find_first_of("/");
        if( p != std::string::npos )
        {
            target = target.substr(0, p);
        }
        
        animationMap[target].push_back(animation);
    }
    
    int animationCount = (int)animationMap.size();
    fwrite(&animationCount, sizeof(int), 1, fp);
    
    std::map<std::string, std::vector<TColladaAnimation*>>::iterator ait = animationMap.begin();
    for(;ait!=animationMap.end();++ait)
    {
        std::vector<TColladaAnimation*>& animations = ait->second;
        if( !animations.empty())
        {
            std::string name = ait->first;
            int nameLength = (int)name.length();
            fwrite(&nameLength, sizeof(int), 1, fp);
            fwrite(name.c_str(), nameLength, 1, fp);
            int frameCount = animations[0]->GetFrameCount();

            fwrite(&frameCount, sizeof(int), 1, fp);
            for(int f=0;f<frameCount;++f)
            {
                Matrix4 transform;
                GetCombinedTransform(animations, f, transform);
                fwrite(transform._m , sizeof(float), 16, fp);
            }
        }
    }
    
    fclose(fp);
}


void TColladaToGASExporter::ExportStaticMesh(TColladaModel* model, const char* path)
{
    
    TColladaMesh* mesh = model->geometrys[0]->mesh;
    
    std::vector<TGBOTriangle> triangles;
    
    std::vector<TVertexItem> vertexItems;
    
    for(int j=0;j<mesh->polylists.size();++j)
    {
        TColladaPolyList* polylist = mesh->polylists[j];
        polylist->IsValid();
        // skip the #
        size_t length = polylist->vertexSource.length();
        std::string s = polylist->vertexSource.substr(1, length-1);
        
        TColladaSource* vertexSource = mesh->FindVertexSourceById(s);
        
        assert( vertexSource );
        length = polylist->texCoordSource.length();
        s = polylist->texCoordSource.substr(1, length-1);
        TColladaSource* textureSource = mesh->FindSourceById(s);

        
        int startIndex = 0;
        for(int v=0;v<polylist->vcount.size();++v)
        {
            int vertexCount = polylist->vcount[v];
            for(int k=2;k<vertexCount;++k)
            {
                int stride = 3;
                
                int vv[3];
                vv[0] = 0;
                vv[1] = k - 1;
                vv[2] = k;
                
                int triIndex[3];
                int uvIndex[3];
                
                
                Vector3 position[3];
                Vector2 uvs[3];
                
                for(int vi=0;vi<3;++vi)
                {
                    int i = (vv[vi]) * stride + startIndex;
                    int vertexID = polylist->p[i];
                    triIndex[vi] = vertexID;
                    int ii = vertexID * 3; // x, y, z
                    position[vi].x = vertexSource->values[ii + 0];
                    position[vi].y = vertexSource->values[ii + 1];
                    position[vi].z = vertexSource->values[ii + 2];

                    int t0 = polylist->textureOffset + (vv[vi]) * stride + startIndex;
                    uvIndex[vi] = polylist->p[t0] * 2; // 2 = u + v
                    uvs[vi].x = textureSource->values[uvIndex[vi]];
                    uvs[vi].y = textureSource->values[uvIndex[vi]+1];
                }
                
                TVertexItem items[3];
                for(int it=0;it<3;++it)
                {
                    items[it].vertex = triIndex[it];
                    items[it].uv = uvIndex[it];
                    items[it].x = position[it].x;
                    items[it].y = position[it].y;
                    items[it].z = position[it].z;
                    items[it].u = uvs[it].x;
                    items[it].v = uvs[it].y;
                }
                
                int id0 = UpdateVertexItem(vertexItems, items[0]);
                int id1 = UpdateVertexItem(vertexItems, items[1]);
                int id2 = UpdateVertexItem(vertexItems, items[2]);
                
                assert( id0 >= 0 );
                assert( id1 >= 0 );
                assert( id2 >= 0 );
                
                //TGASTriangle triangle(vertexItems[id0].vertex, vertexItems[id1].vertex, vertexItems[id2].vertex);
                
                TGBOTriangle triangle(id0, id1, id2);
                triangles.push_back(triangle);
            }
            startIndex += vertexCount * polylist->stride; // vertex, normal , texture
        }
    }
    
    
    TColladaVisualScene* visualScene = model->visualScenes[0];
    
    int jointCount = (int) visualScene->jointNodes.size();
    
    FILE* fp = fopen(path, "wt");
    assert( fp );
    if( ! fp )
        return;

    fprintf(fp, "MD5Version 10\n");
    
    // TODO:
    int meshCount = 1;
    
    fprintf(fp, "numJoints %d\n", jointCount); //TODO: HACK
    fprintf(fp, "numMeshes %d\n", meshCount);
    
    // joints
    fprintf(fp, "joints {\n");
    for(int j=0;j<jointCount;++j)
    {
        if( visualScene->jointNodes[j]->IsRootNode())
        {
            visualScene->jointNodes[j]->UpdateTransform();
        }
    }
    for(int j=1;j<jointCount;++j) //TODO: HACK
    {
        TColladaNode* joint = visualScene->jointNodes[j];
        assert( joint->type == std::string("JOINT"));
        
        Vector3 position = joint->baseTransform.getTrans();
        
        
        Matrix3 mat3;
        joint->baseTransform.extract3x3Matrix(mat3);
        Quaternion q;
        q.FromRotationMatrix(mat3);
        q.normalise();
        
        fprintf(fp, "\t \"%s\" %d ( %f %f %f ) ( %f %f %f )\n",
                joint->id.c_str(),
                joint->parentId,
                position.x, position.y, position.z,
                q.x, q.y, q.z);
    }
    fprintf(fp, "}\n");
    
    
    int vertexCount = (int)vertexItems.size();
    fprintf(fp, "mesh {\n");
    fprintf(fp, "\t numverts %d\n", vertexCount);
    
    
    for(int i=0;i<(int)vertexItems.size();++i)
    {
        int startWeight = 1;
        int weightCount = 0;
        
        float u = vertexItems[i].u;
        float v = vertexItems[i].v;
        float x = vertexItems[i].x;
        float y = vertexItems[i].y;
        float z = vertexItems[i].z;
        fprintf(fp, "\t vert %d ( %f %f %f ) ( %f %f ) %d %d \n", i, x, y, z, u, v, startWeight, weightCount);
    }
    int triangleCount = (int)triangles.size();
    
    fprintf(fp, "\n");
    fprintf(fp, "numtris %d\n", triangleCount);
    for(int triIndex=0;triIndex<triangleCount;++triIndex)
    {
        int id0 = triangles[triIndex].v0;
        int id1 = triangles[triIndex].v1;
        int id2 = triangles[triIndex].v2;
        fprintf(fp, "\t tri %d %d %d %d\n", triIndex, id0, id1, id2);
    }
    
    
    
#if 0
    int weightCount = (int)weightSource->values.size();
    fprintf(fp, "\n");
    fprintf(fp, "numweights %d\n", weightCount);
    
    
    int jointOffset = vertexWeights->startJointIndices[0];
    
    for(int j=0;j<(int)weightSource->values.size();++j)
    {
        int jointIndex = vertexWeights->v[jointOffset];
        
        int weightIndex = vertexWeights->v[jointOffset + 1];
        float weight = weightSource->values[weightIndex];
        
        
        fprintf(fp, "\t weight %d %d %f\n", j , jointIndex, weight); // HACK j+1
        
        
        jointOffset += 2;
    }
#endif
    
    fprintf(fp, "\n");
    
    fprintf(fp, "}\n");
    
    
    
    fclose(fp);
}




bool TColladaToGASExporter::CanBakeUVToVertexWithoutUsingNewVertex(TColladaMesh* mesh) const
{
    for(int j=0;j<mesh->polylists.size();++j)
    {
        TColladaPolyList* list = mesh->polylists[j];
        list->IsValid();
        
        
        std::vector<TTriangleInfo> tts;
      //  int t = list->Triangulate(tts);
        
        // skip the #
        size_t length = list->vertexSource.length();
        std::string s = list->vertexSource.substr(1, length-1);
        
        TColladaSource* vs = mesh->FindVertexSourceById(s);
        
        assert( vs );
        length = list->texCoordSource.length();
        s = list->texCoordSource.substr(1, length-1);
        //TColladaSource* textureSource = mesh->FindSourceById(s);
        
       // int offset = 0;
      // int startIndex = offset;
        
        
        // loop thought the p list
        int vertexCount = (int)list->p.size() / list->stride;
        
        std::map<int, std::set<int>> vertexToUVMap;
        int index = 0;
        for(int v=0;v<vertexCount;++v)
        {
            int vertex = list->p[index];
          //  int normal = list->p[index+1];
            int uv = list->p[index+2];
            
            std::map<int, std::set<int>>::iterator it = vertexToUVMap.find(vertex);
            if( it == vertexToUVMap.end())
            {
                std::set<int> newSet;
                newSet.insert(uv);
                vertexToUVMap[vertex] = newSet;
            }
            else
            {
                it->second.insert(uv);
            }
            
            index += list->stride;
        }
        
        std::map<int, std::set<int>>::iterator it = vertexToUVMap.begin();
        for(;it!=vertexToUVMap.end();++it)
        {
            std::set<int>::iterator sit = it->second.begin();
            printf("%d: ", it->first);
            for(;sit!=it->second.end();++sit)
            {
                int v = *sit;
                printf("%d ", v);
            }
            printf("\n");
        }
        it = vertexToUVMap.begin();
        for(;it!=vertexToUVMap.end();++it)
        {
            if( it->second.size() != 1 )
                return false;
        }
    }
    
    return true;
}

int TColladaToGASExporter::GetJointInNameID(TColladaNode* joint, TColladaSkin* skin) const
{
    TColladaSource* jointNameSource = skin->GetJointNameSource();
    assert( jointNameSource );
    
    int jointCount = (int) jointNameSource->nameArray->names.size();
    for(int j=0;j<jointCount;++j)
    {
        if( joint->name == jointNameSource->nameArray->names[j])
            return j;
    }
    
    return -1;
}


bool TColladaToGASExporter::ExportAnimation(TColladaModel* model, const char* path)
{
    std::vector<TColladaAnimation*> animations;
    model->CollectAnimations(animations);
    if( animations.empty())
        return false;
    
    FILE* fp = fopen(path, "wt");
    assert( fp );
    if( ! fp )
        return false;
    
  // assert( model->controllers.size() == 1 );
    fprintf(fp, "GASVersion 10\n");
    fprintf(fp, "\n\n");

    
    for(int con=0;con<model->controllers.size();++con)
    {
        
    TColladaSkin* skin = model->controllers[con]->skin;
    assert( skin );
    
    assert( !model->visualScenes.empty() );
    
    TColladaVisualScene* visualScene = model->visualScenes[0];
    
    TColladaSource* jointNameSource = skin->GetJointNameSource();
    assert( jointNameSource );
    
    int jointCount = (int) jointNameSource->nameArray->names.size();
    
      
        
    PrepareAnimations(model, 0);


    // TODO:
        
    int meshCount = 1;
        
    int numFrames = 0;
    for(int anim=0;anim<animations.size();++anim)
    {
        TColladaAnimation* animation = animations[anim]; // HACK
        TColladaSource* timeSource = animation->GetTimeSource();
        if( timeSource )
        {
            numFrames = (int)timeSource->values.size();
            break;
        }
    }
        
        
    int frameRate = 30;
    int numAnimatedComponents = (int)nodeTransforms.size() * 6;
        
    fprintf(fp, "animation {\n");
        
    
    fprintf(fp, "numFrames %d\n", numFrames);
    fprintf(fp, "numJoints %d\n", jointCount);
    fprintf(fp, "frameRate %d\n", frameRate);
    fprintf(fp, "numAnimatedComponents %d\n", numAnimatedComponents);
    
    fprintf(fp, "\n\n\n");
    // hierarchy
    fprintf(fp, "hierarchy {\n");
    
    int treeCount = 0;
    for(int j=0;j<jointCount;++j)
    {
        if( visualScene->jointNodes[j]->IsRootNode())
        {
            visualScene->jointNodes[j]->UpdateTransform();
            ++ treeCount;
        }
    }
    
    
    // fix issue
    // some collada file will list the joints NOT in the parent first order
    // GAS require joints has to be listed in parent first order, fix this issue here
    
    std::map<int, TColladaNode*> jointMap;
    std::map<TColladaNode*, int> jointToNameMap;
    
    int treeSize = 0;
    std::queue<TColladaNode*> st;
    for(int j=0;j<visualScene->jointNodes.size();++j)
    {
        TColladaNode* node = visualScene->jointNodes[j];
        int nodeId = 0;
        if( node->IsRootNode())
        {
            st.push(node);
            while(!st.empty())
            {
                TColladaNode* top = st.front();
                ++ treeSize;
                st.pop();
                top->nodeId = nodeId;
                if( GetJointInNameID(top, skin) >= 0 )
                {
                    ++ nodeId;
                }
                for(int child=0;child<(int)top->childs.size();++child)
                {
                    st.push(top->childs[child]);
                }
            }
        }
    }
    
    std::map<int, int> redirectMap;
    std::map<TColladaNode*, Matrix4> inverseBindPoseMap;
    for(int j=0;j<jointCount;++j)
    {
        const std::string& jointName = jointNameSource->nameArray->names[j];
        TColladaNode* joint = visualScene->GetJointNodeBySID(jointName);
        inverseBindPoseMap[joint] = skin->invBindPoses[j];
        jointToNameMap[joint] = j;
        jointMap[joint->nodeId] = joint;
        
        redirectMap[j] = joint->nodeId;
    }
    
    int startIndex = 0;
    std::map<int, TColladaNode*>::iterator it = jointMap.begin();
        int kk = 0;
    for(;it!=jointMap.end();++it)
    {
        TColladaNode* joint = it->second;
        int flags = 0;
        int items = 0;
        std::map<TColladaNode*, std::vector<TAnimTransform>>::iterator it = nodeTransforms.find(joint);
        if( it != nodeTransforms.end())
        {
            if( it->second.size()==1 && it->second[0].type == TAnimTransform::Matrix )
            {
                flags = 63;
                items = 6;
            }
            else
            {
                for(size_t at=0;at<it->second.size();++at)
                {
                    TAnimTransform anim = it->second[at];
                    flags += anim.type;
                    ++ items;
                }
            }
        }
        
        assert( joint->type == std::string("JOINT"));
        
        int sindex = startIndex;
        if( flags == 0 )
            sindex = 0;
        
        int parentId = -1;
        TColladaNode* parentJoint = joint->parentNode;
        if( parentJoint )
        {
            parentId = parentJoint->nodeId;
            assert( parentId >= 0);
        }
        
        
        //if( kk == 0 ) //HACK
          //  parentId = -1;
        
        
        ++kk;
        fprintf(fp, "\t\"%s\" %d %d %d\n", joint->id.c_str(), parentId, flags, sindex);
        
        if( flags != 0 )
            startIndex += items;
        
    }

    
    fprintf(fp, "}\n");
    fprintf(fp, "\n\n\n");
    
    // bounds
    fprintf(fp, "bounds {\n");
    for(int j=0;j<numFrames;++j)
    {
        // TODO: calculate the bounds
        float x0 = -1.0f;
        float y0 = -1.0f;
        float z0 = -1.0f;
        float x1 = 1.0;
        float y1 = 1.0f;
        float z1 = 1.0f;
        
        fprintf(fp, "\t( %f %f %f ) ( %f %f %f )\n", x0, y0, z0, x1, y1, z1);
    }
    fprintf(fp, "}\n");
    fprintf(fp, "\n\n\n");
    
    // baseframe
    fprintf(fp, "baseframe {\n");
    it = jointMap.begin();
    for(;it!=jointMap.end();++it)
    {
        TColladaNode* joint = it->second;
        Matrix4 inverseBindPose = inverseBindPoseMap[joint];
        TColladaNode* parentJoint = joint->parentNode;
        Matrix4 transform = inverseBindPose.inverse();
        if( parentJoint )
        {
            Matrix4 parentInverseBindPose = inverseBindPoseMap[parentJoint];
            Matrix4 parentTransform = parentInverseBindPose;
            transform = parentTransform * transform;
        }
        
        Vector3 pos = transform.getTrans();
        
        Matrix3 mat3;
        
        transform.extract3x3Matrix(mat3);
        Quaternion q;
        q.FromRotationMatrix(mat3);
        q.normalise();
        
        
        fprintf(fp, "\t( %f %f %f ) ( %f %f %f )\n", pos.x, pos.y, pos.z, q.x, q.y, q.z);
    }
    fprintf(fp, "}\n");
    fprintf(fp, "\n\n\n");

    for(int f=0;f<numFrames;++f)
    {
        fprintf(fp, "frame %d {\n", f);
        PrepareAnimations(model, f);
        //assert( numAnimatedComponents == 6 * (int)nodeTransforms.size());
        //for(int j=0;j<(int)visualScene->jointNodes.size();++j) // TODO: HACK
            
        it = jointMap.begin();
        for(;it!=jointMap.end();++it)
        {
            int nameIndex = jointToNameMap[it->second];
            //const std::string& jointName = jointNameSource->nameArray->names[nameIndex];
            
            TColladaNode* joint = it->second;
           // TColladaNode* joint = visualScene->jointNodes[j];
            
            
            
            assert( joint );
            std::map<TColladaNode*, std::vector<TAnimTransform>>::iterator it = nodeTransforms.find(joint);
            ///assert( it != nodeTransforms.end());
            if( it != nodeTransforms.end() )
            {
                for(size_t at=0;at<it->second.size();++at)
                {
                    TAnimTransform anim = it->second[at];
                    if( anim.type == TAnimTransform::Matrix )
                    {
                        float* m = anim.value.matrix;
                        Matrix4 mat(m[0], m[1], m[2], m[3],
                                    m[4], m[5], m[6], m[7],
                                    m[8], m[9], m[10], m[11],
                                    m[12], m[13], m[14], m[15]
                                    );
                        
                        
                        
                        //mat = mat * joint->transform;
                        Matrix3 mat3;
                        mat.extract3x3Matrix(mat3);
                        Quaternion q;
                        q.FromRotationMatrix(mat3);
                        q = q.Inverse();
                        q.normalise();
                        
                        float x = mat.getTrans().x;
                        float y = mat.getTrans().y;
                        float z = mat.getTrans().z;
                        
                        fprintf(fp, "\t %f %f %f %f %f %f", x, y, z, q.x, q.y, q.z);
                    }
                    else
                    {
                        fprintf(fp, "%f", anim.value.single);
                        if( at < (int)it->second.size()-1 )
                            fprintf(fp, " ");
                    }
                }
                fprintf(fp, "\n");
            }
        }
        
        fprintf(fp, "}\n");
        fprintf(fp, "\n\n\n");
    }
        
        
        
        
        
         fprintf(fp, "}\n");
    }
    fclose(fp);
    return true;
}

bool TColladaToGASExporter::IsAllAnimationTargetDifferentNode(TColladaModel* model) const
{
    assert( model->visualScenes.size() == 1 );
    TColladaVisualScene* visualScene = model->visualScenes[0];
    
    std::map<TColladaNode*, int> targetMap;
    
    std::vector<TColladaAnimation*> animations;
    model->CollectAnimations(animations);
    
    for(int j=0;j<(int)animations.size();++j)
    {
        TColladaAnimation* animation = animations[j];
        bool isEqualTimeIntervel = animation->IsEqualTimeInterval();
        
        assert( isEqualTimeIntervel );
        
        //assert( animation->channels.size()==1);
        for(int c=0;c<animation->channels.size();++c)
        {
            TColladaChannel* chan = animation->channels[c];
            std::string source = chan->source;
            
            // skip the #
            std::string s = TColladaModel::SkipStartChar(source);
            
            TColladaSampler* sss = animation->FindSamplerById(s);
            assert( sss );
            
            
            std::string target = chan->target;
            std::string targetChannel = chan->target;
            size_t p = target.find_first_of("/");
            if( p != std::string::npos )
            {
                targetChannel = target.substr(1+p);
                target = target.substr(0, p);
            }
            
            TColladaNode* node = visualScene->FindNodeById(target);
            assert( node );
            
            std::map<TColladaNode*, int>::iterator it = targetMap.find(node);
            if( it == targetMap.end())
            {
                targetMap[node] = 1;
            }
            else
            {
                targetMap[node] ++;
            }
        }
    }
    
    std::map<TColladaNode*, int>::iterator it = targetMap.begin();
    for(;it!=targetMap.end();++it)
    {
        if(it->second > 1 )
            return false;
    }
    return true;
}


void TColladaToGASExporter::GetCombinedTransform(const std::vector<TColladaAnimation*>& animations, int frame, Matrix4& transform)
{
    transform = Matrix4::IDENTITY;
    for(int j=0;j<(int)animations.size();++j)
    {
        Matrix4 t;
        GetAnimationTransform(animations[j],frame, t);
        transform = transform * t;
   }
}

void TColladaToGASExporter::GetAnimationTransform(TColladaAnimation* animation, int frame, Matrix4& transform)
{
    bool isEqualTimeIntervel = animation->IsEqualTimeInterval();
    
    //assert( isEqualTimeIntervel );
    
    //assert( animation->channels.size()==1);
    for(int c=0;c<animation->channels.size();++c)
    {
        TColladaChannel* chan = animation->channels[c];
        std::string source = chan->source;
        
        // skip the #
        std::string s = TColladaModel::SkipStartChar(source);
        
        TColladaSampler* sss = animation->FindSamplerById(s);
        assert( sss );
        
        TColladaSource* inputSource = NULL;
        TColladaSource* outputSource = NULL;
        TColladaSource* inTengentSource = NULL;
        TColladaSource* outTengentSource = NULL;
        TColladaSource* interpolateSource = NULL;
        
        for(int input=0;input<(int)sss->inputs.size();++input)
        {
            TColladaInput* i = sss->inputs[input];
            
            std::string sid = TColladaModel::SkipStartChar(i->source);
            TColladaSource* ss = animation->FindSourceById(sid);
            assert( ss );
            
            if( i->semantic == std::string("INPUT") )
                inputSource = ss;
            else if( i->semantic == std::string("OUTPUT") )
                outputSource = ss;
            else if( i->semantic == std::string("INTERPOLATION") )
                interpolateSource = ss;
            else if( i->semantic == std::string("IN_TANGENT") )
                inTengentSource = ss;
            else if( i->semantic == std::string("OUT_TANGENT") )
                outTengentSource = ss;
        }
        
        std::string target = chan->target;
        std::string targetChannel = chan->target;
        size_t p = target.find_first_of("/");
        if( p != std::string::npos )
        {
            targetChannel = target.substr(1+p);
            target = target.substr(0, p);
        }
        
        
        //TColladaNode* node = visualScene->FindNodeById(target);
        //assert( node );
        
        // float time = 0.0f;
        
        assert( inputSource && outputSource );
        
        
        assert( inputSource->accessor->params.size() == 1 );
        
        if( inputSource->accessor->params[0]->name == "TIME" )
        {
        }
        
        transform = Matrix4::IDENTITY;
        
        Quaternion rotation = Quaternion::IDENTITY;
        
        for(int p=0;p<outputSource->accessor->params.size();++p)
        {
            //assert( outputSource->accessor->params.size()==1);
            TColladaParam* param = outputSource->accessor->params[p];
            
            int stride = outputSource->accessor->stride;
            std::vector<float>& values = outputSource->values;
            
            if( param->name == "TRANSFORM" || targetChannel == "matrix" )
            {
                assert( values.size() % 16 == 0 );
                int frameCount = (int)values.size() / 16;
                assert( frame < frameCount );
                int startIndex = 16 * frame;
                    
                Matrix4 mat = TColladaParserUtils::FloatArrayToMatrix(values, startIndex);
                transform = mat;
            }
            else if( targetChannel == std::string("translate") )
            {
                int offset = outputSource->accessor->GetParamOffset(param);
                assert( offset >= 0 );
                int idx = stride * frame + offset;
                assert( idx<(int)values.size() );
                float value = values[idx];
                if( param->name == "X" )
                {
                    transform = transform * Matrix4::getTrans(value, 0.0f, 0.0f);
                }
                if( param->name == "Y" )
                {
                    transform = transform * Matrix4::getTrans(0.0f, value, 0.0f);
                }
                if( param->name == "Z" )
                {
                    transform = transform * Matrix4::getTrans(0.0f, 0.0f, value);
                }
            }
            else if( targetChannel == std::string("translate.Z"))
            {
                assert( param->name == "Z" );
                int i = frame;
                assert( i < values.size());
                transform = transform * Matrix4::getTrans(0.0f, 0.0f, values[i]);
            }
            else if( targetChannel == std::string("rotateX.ANGLE") )
            {
                assert( outputSource->accessor->stride == 1 );
                int i = frame;
                float angle = values[i];
                assert( i < values.size());
                
                angle = Math::ToRadian(angle);
                
                
                Quaternion qq0;
                qq0.FromAngleAxis(angle, Vector3::UNIT_X);
                qq0.normalise();
                rotation = rotation * qq0;
                
                
                Matrix3 rot;
                qq0.ToRotationMatrix(rot);
                qq0.normalise();
                Matrix4 mat(rot);
                
                transform = transform * mat;
            }
            else if( targetChannel == std::string("rotateY.ANGLE") )
            {
                assert( outputSource->accessor->stride == 1 );
                int i = frame;
                assert( i < values.size());
                float angle = values[i];
                angle = Math::ToRadian(angle);
                
                Quaternion qq0;
                qq0.FromAngleAxis(angle, Vector3::UNIT_Y);
                qq0.normalise();
                rotation = rotation * qq0;
                
                
                Matrix3 rot;
                qq0.ToRotationMatrix(rot);
                Matrix4 mat(rot);
                
                transform = transform * mat;
                
            }
            else if( targetChannel == std::string("rotateZ.ANGLE") )
            {
                assert( outputSource->accessor->stride == 1 );
                int i = frame;
                assert( i < values.size());
                float angle = values[i];
                angle = Math::ToRadian(angle);
                
                Quaternion qq0;
                qq0.FromAngleAxis(angle, Vector3::UNIT_Z);
                rotation = rotation * qq0;
                
                
                Matrix3 rot;
                qq0.ToRotationMatrix(rot);
                Matrix4 mat(rot);
                
                transform = transform * mat;
                
            }
        }
    }
}


void TColladaToGASExporter::GetAnimationTransfrom(TColladaAnimation* animation, std::vector<Matrix4>& transforms)
{
    bool isEqualTimeIntervel = animation->IsEqualTimeInterval();
    
   // assert( isEqualTimeIntervel );
    
    //assert( animation->channels.size()==1);
    for(int c=0;c<animation->channels.size();++c)
    {
        TColladaChannel* chan = animation->channels[c];
        std::string source = chan->source;
        
        // skip the #
        std::string s = TColladaModel::SkipStartChar(source);
        
        TColladaSampler* sss = animation->FindSamplerById(s);
        assert( sss );
        
        TColladaSource* inputSource = NULL;
        TColladaSource* outputSource = NULL;
        TColladaSource* inTengentSource = NULL;
        TColladaSource* outTengentSource = NULL;
        TColladaSource* interpolateSource = NULL;
        
        for(int input=0;input<(int)sss->inputs.size();++input)
        {
            TColladaInput* i = sss->inputs[input];
            
            std::string sid = TColladaModel::SkipStartChar(i->source);
            TColladaSource* ss = animation->FindSourceById(sid);
            assert( ss );
            
            if( i->semantic == std::string("INPUT") )
                inputSource = ss;
            else if( i->semantic == std::string("OUTPUT") )
                outputSource = ss;
            else if( i->semantic == std::string("INTERPOLATION") )
                interpolateSource = ss;
            else if( i->semantic == std::string("IN_TANGENT") )
                inTengentSource = ss;
            else if( i->semantic == std::string("OUT_TANGENT") )
                outTengentSource = ss;
        }
        
        std::string target = chan->target;
        std::string targetChannel = chan->target;
        size_t p = target.find_first_of("/");
        if( p != std::string::npos )
        {
            targetChannel = target.substr(1+p);
            target = target.substr(0, p);
        }
        
        
        //TColladaNode* node = visualScene->FindNodeById(target);
        //assert( node );
        
        // float time = 0.0f;
        
        assert( inputSource && outputSource );
        
        
        assert( inputSource->accessor->params.size() == 1 );
        
        if( inputSource->accessor->params[0]->name == "TIME" )
        {
        }
        
        int stride = outputSource->accessor->stride;
        std::vector<float>& values = outputSource->values;
        
        
        
        
        Matrix4 transform = Matrix4::IDENTITY;
        
        Quaternion rotation = Quaternion::IDENTITY;
        
        for(int p=0;p<outputSource->accessor->params.size();++p)
        {
            //assert( outputSource->accessor->params.size()==1);
            TColladaParam* param = outputSource->accessor->params[p];
            if( param->name == "TRANSFORM" || targetChannel == "matrix" )
            {
                std::vector<float>& values = outputSource->values;
                assert( values.size() % 16 == 0 );
                int frameCount = values.size() / 16;
                for(int frame=0;frame<frameCount;++frame)
                {
                    int startIndex = 16 * frame;
                    
                    Matrix4 mat = TColladaParserUtils::FloatArrayToMatrix(values, startIndex);
                    transforms.push_back(mat);
                }
            }
        }
    }
}

void TColladaToGASExporter::PrepareAnimations(TColladaModel* model, int frame)
{
    assert( ! model->visualScenes.empty() );
    
    nodeTransforms.clear();
    
    
    TColladaVisualScene* visualScene = model->visualScenes[0];
    std::vector<int> frameCounts;
    std::vector<TColladaAnimation*> animations;
    
    model->CollectAnimations(animations);
    
    for(int j=0;j<(int)animations.size();++j)
    {
        TColladaAnimation* animation = animations[j];
        bool isEqualTimeIntervel = animation->IsEqualTimeInterval();
        
        //assert( isEqualTimeIntervel );
        
        //assert( animation->channels.size()==1);
        for(int c=0;c<animation->channels.size();++c)
        {
            TColladaChannel* chan = animation->channels[c];
            std::string source = chan->source;
            
            // skip the #
            std::string s = TColladaModel::SkipStartChar(source);
            
            TColladaSampler* sss = animation->FindSamplerById(s);
            assert( sss );
            
            TColladaSource* inputSource = NULL;
            TColladaSource* outputSource = NULL;
            TColladaSource* inTengentSource = NULL;
            TColladaSource* outTengentSource = NULL;
            TColladaSource* interpolateSource = NULL;
            
            for(int input=0;input<(int)sss->inputs.size();++input)
            {
                TColladaInput* i = sss->inputs[input];
                
                std::string sid = TColladaModel::SkipStartChar(i->source);
                TColladaSource* ss = animation->FindSourceById(sid);
                assert( ss );
                
                if( i->semantic == std::string("INPUT") )
                    inputSource = ss;
                else if( i->semantic == std::string("OUTPUT") )
                    outputSource = ss;
                else if( i->semantic == std::string("INTERPOLATION") )
                    interpolateSource = ss;
                else if( i->semantic == std::string("IN_TANGENT") )
                    inTengentSource = ss;
                else if( i->semantic == std::string("OUT_TANGENT") )
                    outTengentSource = ss;
            }
            
            std::string target = chan->target;
            std::string targetChannel = chan->target;
            size_t p = target.find_first_of("/");
            if( p != std::string::npos )
            {
                targetChannel = target.substr(1+p);
                target = target.substr(0, p);
            }
            
            
            TColladaNode* node = visualScene->FindNodeById(target);
            assert( node );
            
            // float time = 0.0f;
            
            assert( inputSource && outputSource );
            
            
            assert( inputSource->accessor->params.size() == 1 );
            
            if( inputSource->accessor->params[0]->name == "TIME" )
            {
                //  time = inputSource->values[frame];
                frameCounts.push_back(inputSource->values.size());
            }
            
            int stride = outputSource->accessor->stride;
            std::vector<float>& values = outputSource->values;

            
            
            
            Matrix4 transform = Matrix4::IDENTITY;
            
            Quaternion rotation = Quaternion::IDENTITY;

            for(int p=0;p<outputSource->accessor->params.size();++p)
            {
                //assert( outputSource->accessor->params.size()==1);
                TColladaParam* param = outputSource->accessor->params[p];
                if( param->name == "TRANSFORM" || targetChannel == "matrix" )
                {
                    //if( param->type == "float4x4" )
                    {
                        std::vector<float>& values = outputSource->values;
                        int startIndex = 16 * frame;
                        
                        Matrix4 mat = TColladaParserUtils::FloatArrayToMatrix(values, startIndex);
                        transform = transform * mat;
                        
                        TAnimTransform anim;
                        anim.type = TAnimTransform::Matrix;
                        for(int kk=0;kk<16;++kk)
                            anim.value.matrix[kk] = mat._m[kk];
                        nodeTransforms[node].push_back(anim);
                    }
                }
                else if( targetChannel == std::string("translate") )
                {
                    int offset = outputSource->accessor->GetParamOffset(param);
                    assert( offset >= 0 );
                    int idx = stride * frame + offset;
                    assert( idx<(int)values.size() );
                    float value = values[idx];
                    if( param->name == "X" )
                    {
                        transform = transform * Matrix4::getTrans(value, 0.0f, 0.0f);
                    }
                    if( param->name == "Y" )
                    {
                        transform = transform * Matrix4::getTrans(0.0f, value, 0.0f);
                    }
                    if( param->name == "Z" )
                    {
                        transform = transform * Matrix4::getTrans(0.0f, 0.0f, value);
                    }
                }
                else if( targetChannel == std::string("translate.Z"))
                {
                    assert( param->name == "Z" );
                    int i = frame;
                    assert( i < values.size());
                    transform = transform * Matrix4::getTrans(0.0f, 0.0f, values[i]);
                }
                else if( targetChannel == std::string("rotateX.ANGLE") )
                {
                    assert( outputSource->accessor->stride == 1 );
                    int i = frame;
                    float angle = values[i];
                    assert( i < values.size());
                    
                    angle = Math::ToRadian(angle);
                    
                    
                    Quaternion qq0;
                    qq0.FromAngleAxis(angle, Vector3::UNIT_X);
                    qq0.normalise();
                    rotation = rotation * qq0;
                    
                    
                    Matrix3 rot;
                    qq0.ToRotationMatrix(rot);
                    qq0.normalise();
                    Matrix4 mat(rot);
                    
                    transform = transform * mat;
                }
                else if( targetChannel == std::string("rotateY.ANGLE") )
                {
                    assert( outputSource->accessor->stride == 1 );
                    int i = frame;
                    assert( i < values.size());
                    float angle = values[i];
                    angle = Math::ToRadian(angle);
                    
                    Quaternion qq0;
                    qq0.FromAngleAxis(angle, Vector3::UNIT_Y);
                    qq0.normalise();
                    rotation = rotation * qq0;
                    
                    
                    Matrix3 rot;
                    qq0.ToRotationMatrix(rot);
                    Matrix4 mat(rot);
                    
                    transform = transform * mat;
                    
                }
                else if( targetChannel == std::string("rotateZ.ANGLE") )
                {
                    assert( outputSource->accessor->stride == 1 );
                    int i = frame;
                    assert( i < values.size());
                    float angle = values[i];
                    angle = Math::ToRadian(angle);
                    
                    Quaternion qq0;
                    qq0.FromAngleAxis(angle, Vector3::UNIT_Z);
                    rotation = rotation * qq0;
                    
                    
                    Matrix3 rot;
                    qq0.ToRotationMatrix(rot);
                    Matrix4 mat(rot);
                    
                    transform = transform * mat;
                    
                }
                else if( targetChannel == std::string("ANGLE") )
                {
                    //assert( outputSource->accessor->stride == 1 );
                    //Quaternion qq0;
                    //qq0.FromAngleAxis(values[i], Vector3::UNIT_Z);
                    //rotation = rotation * qq0;
                }
                else if( targetChannel == std::string("blendParent1") )
                {
                }
                else
                {
                }
            }
        }
    }
    //for(int j=1;j<(int)frameCounts.size();++j)
    //{
    //    assert( frameCounts[j-1] == frameCounts[j] );
    //}
}


#pragma pack(pop)