#include "GBOModel.h"

#include "GBOAnimation.h"

#include "Graphics/FlatRenderer.h"
#include "Math3D/Vector2.h"

#include "Collada/SlowColladaRenderer.h"

#pragma pack(push)
#pragma pack(1)

struct TGBOChunk
{
    long offset;
    long size;
};

TGBOTriangle::TGBOTriangle()
{
    v0 = -1;
    v1 = -1;
    v2 = -1;
}

TGBOTriangle::TGBOTriangle(int _v0, int _v1, int _v2)
{
    v0 = _v0;
    v1 = _v1;
    v2 = _v2;
}

TGBOWeight::TGBOWeight()
{
    jointIndex = -1;
    weight = -1.0f;
}


TGBOWeight::TGBOWeight(int jIndex, float w)
{
    jointIndex = jIndex;
    weight = w;
}


TGBOMesh::TGBOMesh()
{
    isVisible = true;
}

void TGBOMesh::SetVisible(bool v)
{
    isVisible = v;
}


bool TGBOMesh::IsVisible() const
{
    return isVisible;
}



TGBOModel::TGBOModel()
{
    animation = NULL;
}

bool TGBOModel::SetAnimation(TGBOAnimation* _animation)
{
    if( !_animation )
        return false;
    
    if( joints.size() != _animation->jointInfos.size() )
        return false;
    
    //assert( joints.size() == animation->jointInfos.size());
    for(int j=0;j<(int)_animation->jointInfos.size();++j)
    {
        std::string& jointName = joints[j]->name;
        if( _animation->jointInfos[j].name != jointName )
            return false;
    }
    
    animation = _animation;

    return true;
}

Quaternion TGBOModel::BuildQuaternion(float qx, float qy, float qz)
{
    float t = 1.0f - qx * qx - qy * qy - qz * qz;
    float qw = 0.0f;
    if ( t < 0.0f )
    {
        qw = 0.0f;
    }
    else
    {
        qw =  sqrtf(t);
    }
    Quaternion q(qw, qx, qy, qz);
    return q;
}


#pragma pack (1)

inline void SwapEndian(int &val)
{
    val = (val<<24) | ((val<<8) & 0x00ff0000) |
		  ((val>>8) & 0x0000ff00) | (val>>24);
}


bool TGBOModel::LoadAscii(const char *filename)
{
    FILE*fp = fopen(filename, "rt");
    assert( fp );
    

    if( ! fp )
    {
        //	TDebugDrawer::Print("WARNING!!!! %s Loading failed! %s %s", path,  __FILE__ , __LINE__);
        return false;
    }
    char line[512];
    while( fgets(line, 512, fp) )
    {
        char token[512] = { 0 };
        int s = sscanf(line, "%s", token);
        if( s != EOF )
        {
            if( 0 == strcmp(token, "joints") )
            {
                ParseJoints(fp);
            }
            else if( 0==strcmp(token, "mesh") )
            {
                LoadSubMesh(fp);
            }
        }
    }
    
    for(size_t j=0;j<joints.size();++j)
    {
        TGBOJoint* joint = joints[j];
        if( joint->parentId >= 0 )
        {
            assert( joint->parentId < j );
            TGBOJoint* parent = joints[joint->parentId];
            parent->AddChild(joint);
        }
    }
    
    
    // The MD5 file format do NOT store the vertex bind pose position
    // we calculate the vertex position and store it
    //PrepareBindPoseVertices();
    
    
    
    for(size_t j=0;j<joints.size();++j)
    {
        if( ! joints[j]->parent )
        {
            joints[j]->Dump();
        }
    }
    
    
  
    // build the smoothed normal of each vertex
    for(int j=0;j<(int)meshes.size();++j)
    {
        TGBOMesh* mesh = meshes[j];
        for(int v=0;v<(int)mesh->vertices.size();++v)
        {
            mesh->vertices[v].normal = Vector3::ZERO;
            mesh->vertices[v].tangent = Vector3::ZERO;
        }
        for(int tri=0;tri<(int)mesh->triangles.size();++tri)
        {
            TGBOVertex& v0 = mesh->vertices[mesh->triangles[tri].v0];
            TGBOVertex& v1 = mesh->vertices[mesh->triangles[tri].v1];
            TGBOVertex& v2 = mesh->vertices[mesh->triangles[tri].v2];
            Vector3 edge0 = v1.bindPosePosition - v0.bindPosePosition;
            Vector3 edge1 = v2.bindPosePosition - v0.bindPosePosition;
            Vector3 normal = edge0.crossProduct(edge1);
            v0.normal += normal;
            v1.normal += normal;
            v2.normal += normal;
            
            
            Vector2 uv0 = Vector2(v0.u, v0.v);
            Vector2 uv1 = Vector2(v1.u, v1.v);
            Vector2 uv2 = Vector2(v2.u, v2.v);
            
            Vector3 deltaPos1 = v1.bindPosePosition - v0.bindPosePosition;
            Vector3 deltaPos2 = v2.bindPosePosition - v0.bindPosePosition;
            
            // UV delta
            Vector2 deltaUV1 = uv1-uv0;
            Vector2 deltaUV2 = uv2-uv0;
            
            float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
            Vector3 tangent = (deltaPos1 * deltaUV2.y   - deltaPos2 * deltaUV1.y)*r;
            //Vector3 bitangent = (deltaPos2 * deltaUV1.x   - deltaPos1 * deltaUV2.x)*r;
            
            v0.tangent += tangent;
            v1.tangent += tangent;
            v2.tangent += tangent;
        }
        for(int v=0;v<(int)mesh->vertices.size();++v)
        {
            mesh->vertices[v].normal.normalise();
            
            Vector3 normal = mesh->vertices[v].normal;
            Vector3 tangent = mesh->vertices[v].tangent;
            
            
            mesh->vertices[v].tangent = (tangent - normal * normal.dotProduct(tangent));
            mesh->vertices[v].tangent.normalise();
        }
    }
    
    
    return true;
}

void TGBOModel::ParseJoints(FILE* fp)
{
    char line[512];
    while( fgets(line, 512, fp) )
    {
        if( line[0] == '}' )
            return;
        else
        {
            char jointName[512] = { 0 };
            char dummy = { 0 };
            int parentId = 0;
            float pos[3];
            float orientation[3];
            float invPos[16];
            //float invOri[3];
            int s = sscanf(line, "%s %d %c %f %f %f %c %c %f %f %f %c %c %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %c", jointName, &parentId, &dummy, &pos[0], &pos[1], &pos[2], &dummy, &dummy, &orientation[0], &orientation[1], &orientation[2], &dummy, &dummy,
                           
                           &invPos[0], &invPos[1], &invPos[2], &invPos[3], 
                           &invPos[4], &invPos[5], &invPos[6], &invPos[7],
                           &invPos[8], &invPos[9], &invPos[10], &invPos[11],
                           &invPos[12], &invPos[13], &invPos[14], &invPos[15],
                           &dummy);
            
           // assert( s == 22 );
            if( s != EOF )
            {
                TGBOJoint* joint = new TGBOJoint();
                joint->name = std::string(jointName);
                size_t length = joint->name.length();
                joint->name = joint->name.substr(1, length-2); // skip the double quote
                joint->parentId = parentId;
                
                //TODO:
                
                
                joint->position = Vector3(pos[0],pos[1],pos[2]);
                joint->orientation = BuildQuaternion(orientation[0], orientation[1], orientation[2]);
                
                Matrix3 rot;
                joint->orientation.ToRotationMatrix(rot);
                
                Matrix4 transform = Matrix4(rot);
                transform.setTrans(joint->position);
                joint->transform = transform;
                joint->worldTransform = transform;
                
                
                
                //Quaternion q = BuildQuaternion(invOri[0], invOri[1], invOri[2]);
                //q.ToRotationMatrix(rot);
                
               // joint->inverseBindPose = Matrix4(rot);
                //joint->inverseBindPose.setTrans(Vector3(invPos[0], invPos[1], invPos[2]));
                
                //joint->inverseBindPose = joint->inverseBindPose.inverse();
                
                joint->inverseBindPose = Matrix4(invPos[0], invPos[1], invPos[2], invPos[3],
                                                 invPos[4], invPos[5], invPos[6], invPos[7],
                                                 invPos[8], invPos[9], invPos[10], invPos[11],
                                                 invPos[12], invPos[13], invPos[14], invPos[15]
                                                 
                                                 );
                joints.push_back(joint);
                
                
                
                
                
                
                
                
            }
        }
        
        //"origin"	-1 ( 0 0 0 ) ( -0.7071067095 0 0 )
        
    }
}

void TGBOModel::LoadSubMesh(FILE* fp)
{
    char line[512];
    int maxWeightCount = 0;
    
    TGBOMesh* mesh = new TGBOMesh();
    while( fgets(line, 512, fp) )
    {
        if( line[0] == '}' )
            break;
        else
        {
            char token[512] = { 0 };
            int s = sscanf(line, "%s", token);
            if( s != EOF )
            {
                if( 0 == strcmp("numverts", token) )
                {
                    
                }
                else if( 0 == strcmp("vert", token) )
                {
                    char name[256];
                    char dummy = { 0 };
                    float x = 0.0f;
                    float y = 0.0f;
                    float z = 0.0f;
                    float u = -1.0f;
                    float v = -1.0f;
                    int index = 0;
                    int startWeight;
                    int weightCount;
                    int k = sscanf(line, "%s %d %c %f %f %f %c %c %f %f %c %d %d", name, &index, &dummy, &x, &y, &z, &dummy, &dummy, &u, &v, &dummy, &startWeight, &weightCount);
                    
                    assert( k != EOF);
                    assert( k == 13 );
                    TGBOVertex vertex;
                    vertex.u = u;
                    vertex.v = v;
                    vertex.bindPosePosition = Vector3(x, y, z);
                    vertex.weightStart = startWeight;
                    vertex.weightCount = weightCount;
                    
                    
                    maxWeightCount = vertex.weightCount>maxWeightCount ? vertex.weightCount : maxWeightCount;
                    
                    mesh->vertices.push_back(vertex);
                    
                }
                else if( 0 == strcmp("tri", token) )
                {
                    char dummy[512] = { 0 };
                    int index = 0;
                    int t0, t1, t2;
                    int k = sscanf(line, "%s %d %d %d %d", dummy, &index, &t0, &t1, &t2);
                    
                    assert( k != EOF );
                    assert( k == 5 );
                    mesh->triangles.push_back(TGBOTriangle(t0, t1, t2));
                }
                else if( 0 == strcmp("weight", token) )
                {
                    char dummy[512] = { 0 };
                    int weightIndex;
                    int jointIndex;
                    float weightBias;
                    int k = sscanf(line, "%s %d %d %f", dummy, &weightIndex, &jointIndex, &weightBias);
                    
                    assert( k == 4 );
                    
                    assert( k != EOF );
                    
                    TGBOWeight weight;
                    weight.jointIndex = jointIndex;
                    weight.weight = weightBias;
                    mesh->weights.push_back(weight);
                }
            }
        }
    }
    printf("**************************************************\n");
    printf("Joint count:%ld\n", joints.size());
    printf("Vertex Count:%ld\n", mesh->vertices.size());
    printf("Triangle Count:%ld\n", mesh->triangles.size());
    printf("Weight Count:%ld\n", mesh->weights.size());
    printf("Max Weight Count:%d\n", maxWeightCount);
    printf("**************************************************\n");
    
    meshes.push_back(mesh);
    
}

void TGBOModel::Load(const char* filename)
{

#pragma pack(push)
#pragma pack(1)

    FILE* fp = fopen(filename, "r");
    
    assert( fp );
    
    
    
    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    rewind (fp);
    
    
    TGBOHeader header;
    int readed = fread(&header, sizeof(TGBOHeader), 1, fp);
    
    assert( readed == 1);
    
    int jointCount = 0;
    fread(&jointCount, sizeof(int), 1, fp);
    for(int j=0;j<jointCount;++j)
    {
        TGBOJoint* joint = new TGBOJoint();
        
        int nameLength = 0;
        fread(&nameLength, sizeof(int),1 , fp);
        char name[256];
        memset(name, 0, 256);
        fread(name, nameLength, 1, fp);
        printf("%s\n", name);
        
        
        int parentId = -1;
        fread(&parentId, sizeof(int), 1, fp);
        joint->parentId = parentId;
        
        joint->name = std::string(name);
        
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float ox = 0.0f;
        float oy = 0.0f;
        float oz = 0.0f;
        fread(&x, sizeof(float), 1, fp);
        fread(&y, sizeof(float), 1, fp);
        fread(&z, sizeof(float), 1, fp);
        
        fread(&ox, sizeof(float), 1, fp);
        fread(&oy, sizeof(float), 1, fp);
        fread(&oz, sizeof(float), 1, fp);
        
        joint->position = Vector3(x,y,z);
        joint->orientation = BuildQuaternion(ox, oy, oz);
        
        Matrix3 rot;
        joint->orientation.ToRotationMatrix(rot);
        
        Matrix4 transform = Matrix4(rot);
        transform.setTrans(joint->position);
        joint->transform = transform;
        joint->worldTransform = transform;
        
        // read the inverse bind pose
        fread(&x, sizeof(float), 1, fp);
        fread(&y, sizeof(float), 1, fp);
        fread(&z, sizeof(float), 1, fp);
        
        fread(&ox, sizeof(float), 1, fp);
        fread(&oy, sizeof(float), 1, fp);
        fread(&oz, sizeof(float), 1, fp);
        
        Quaternion q = BuildQuaternion(ox, oy, oz);
        q.ToRotationMatrix(rot);
        
        joint->inverseBindPose = Matrix4(rot);
        joint->inverseBindPose.setTrans(Vector3(x, y, z));
        
        joints.push_back(joint);
    }
    
    for(int j=0;j<(int)joints.size();++j)
    {
        assert( joints[j]->parentId < j );
        
        TGBOJoint* joint = joints[j];
        if( joint->parentId>=0 )
        {
            TGBOJoint* parentJoint = joints[joint->parentId];
            assert( parentJoint );
            parentJoint->AddChild(joint);
        }
    }
    
    int meshCount = 0;
    readed = fread(&meshCount, sizeof(int), 1, fp);
    
    assert( readed == 1 );
    
    for(int j=0;j<meshCount;++j)
    {
        TGBOMesh* mesh = new TGBOMesh();
        meshes.push_back(mesh);
        int vertexCount = 0;
        
        
        size_t read = fread(&vertexCount, sizeof(int), 1, fp);
        assert( read == 1 );
        
        for(int vertex=0;vertex<vertexCount;++vertex)
        {
            float x = 0.0f;
            float y = 0.0f;
            float z = 0.0f;
            float u = 0.0f;
            float v = 0.0f;
            int weightStart = 0;
            int weightCount = 0;
            //read = fread(&x, sizeof(x), 1, fp);
            assert( read == 1 );
            
            //read = fread(&y, sizeof(y), 1, fp);
            assert( read == 1 );
            
            //read = fread(&z, sizeof(z), 1, fp);
            assert( read == 1 );
            
            //read = fread(&u, sizeof(u), 1, fp);
            assert( read == 1 );
            
            //read = fread(&v, sizeof(v), 1, fp);
            assert( read == 1 );
            
            //read = fread(&weightStart, sizeof(weightStart), 1, fp);
            assert( read == 1 );
            
            //read = fread(&weightCount, sizeof(weightCount), 1, fp);
            assert( read == 1 );
            
            int dummy = 0;
            //read = fread(&dummy, sizeof(dummy), 1, fp);
            assert( read == 1 );
            
            TGBOVertex vv;
            vv.bindPosePosition = Vector3(x, y, z);
            vv.u = u;
            vv.v = v;
            vv.weightStart = weightStart;
            vv.weightCount = weightCount;
            
            mesh->vertices.push_back(vv);
        }
        
        //long p = ftell(fp);
        int triangleCount = 0;
        read = fread(&triangleCount, sizeof(int), 1, fp );
        
        
        int tt = triangleCount;
        SwapEndian(tt);
        assert( read == 1 );
        
        for(int t=0;t<triangleCount;++t)
        {
            int vid0 = 0;
            int vid1 = 0;
            int vid2 = 0;
            fread(&vid0, sizeof(int), 1, fp);
            fread(&vid1, sizeof(int), 1, fp);
            fread(&vid2, sizeof(int), 1, fp);
            
            TGBOTriangle triangle;
            triangle.v0 = vid0;
            triangle.v1 = vid1;
            triangle.v2 = vid2;
            mesh->triangles.push_back(triangle);
        }
        
        /*
        int weightCount = 0;
        fread(&weightCount, sizeof(int), 1, fp);
        for(int w=0;w<weightCount;++w)
        {
            TGBOWeight weight;
            fread(&weight.jointIndex, sizeof(int), 1, fp);
            fread(&weight.weight, sizeof(float), 1, fp);
            mesh->weights.push_back(weight);
        }
         */
    }
    
    fclose(fp );
    
    
#pragma pack(pop)

}




void TGBOModel::RenderJoints()
{
    TFlatRenderer* renderer = TFlatRenderer::GetInstance();
    
    int jCount = (int)joints.size();
    for(int j=0;j<jCount;++j)
    {
        TGBOJoint* jj = joints[j];
        int pid = jj->parentId;
        if( pid < 0 )
            continue;
        TGBOJoint* pj = joints[pid];
        
        Vector3 pt0 = jj->worldTransform.getTrans();
        Vector3 pt1 = pj->worldTransform.getTrans();
        
        Vector3 dir = (pt1 - pt0);
        
        //float length = dir.length();
        dir.normalise();
        
        
        Vector3 N = OrthogonalVector(dir.normalisedCopy());
        
        RenderCylinder(pt0, pt1, N, 1.0f);
        
        
        
    }
    renderer->Flush();
}




void TGBOModel::RenderStaticMesh()
{
    TFlatRenderer* renderer = TFlatRenderer::GetInstance();
    
    for(int m=0;m<meshes.size();++m)
    {
        TGBOMesh* mesh = meshes[m];
        for(int j=0;j<mesh->triangles.size();++j)
        {
            TGBOVertex& v0 = mesh->vertices[mesh->triangles[j].v0];
            TGBOVertex& v1 = mesh->vertices[mesh->triangles[j].v1];
            TGBOVertex& v2 = mesh->vertices[mesh->triangles[j].v2];
            
            TFlatRenderer::TVertexData pt0(v0.bindPosePosition.x, v0.bindPosePosition.y, v0.bindPosePosition.z, v0.u, v0.v);
            TFlatRenderer::TVertexData pt1(v1.bindPosePosition.x, v1.bindPosePosition.y, v1.bindPosePosition.z, v1.u, v1.v);
            TFlatRenderer::TVertexData pt2(v2.bindPosePosition.x, v2.bindPosePosition.y, v2.bindPosePosition.z, v2.u, v2.v);
            
            renderer->RenderTriangle(pt0, pt1, pt2);
        }
        renderer->Flush();
    }
    
}

void TGBOModel::Update(float dt)
{
    playedTime += dt;
    UpdateJoints();
    
    for(int j=0;j<(int)joints.size();++j)
    {
        if( joints[j]->parentId < 0 )
            joints[j]->UpdateTransform();
    }
}

void TGBOModel::Eval(const TGBOJointInfo& info, int frame, Vector3& pos, Vector3& orientation)
{
    assert( animation );
    
    
}


TGBOJoint* TGBOModel::GetJointByName(const std::string& name) const
{
    for(int j=0;j<(int)joints.size();++j)
        if( joints[j]->name == name )
            return joints[j];
    
    return NULL;
}



void TGBOModel::UpdateJoints()
{
    
    if( ! animation )
        return;
    
    
    //float fps = mAnimation->mFrameRate;
    int frameCount = (int)animation->animationItems[0].data.size(); // HACK
    assert( frameCount > 0 );
    float totalTime = (1.0f / animation->frameRate) * frameCount;
    
    
    if( playedTime > totalTime )
        playedTime = fmod(playedTime, totalTime);
    
   
    float t = fmod(playedTime, totalTime);
    t /= totalTime;
    
    assert( t>=0.0f && t<=1.0f );
    
    int frame0 = (int)(t * frameCount);
    
    if( frame0 >= frameCount )
        frame0 = 0;
    
    int frame1 = frame0 + 1;
    if( frame1 >= frameCount )
        frame1 = frameCount-1;
    
     
    for(int anim=0;anim<(int)animation->animationItems.size();++anim)
    {
        TAnimationItem& item = animation->animationItems[anim];
        
        TGBOJoint* joint = GetJointByName(item.target);
        assert( joint );
        
        Matrix4 transform = item.data[frame0].transform;
        Vector3 position = transform.getTrans();
        
        Matrix3 rotation;
        transform.extract3x3Matrix(rotation);
        
        Quaternion orientation;
        orientation.FromRotationMatrix(rotation);
        
        /*
        if( joint->parentId >= 0 )
        {
            TGBOJoint* parentJoint = joints[joint->parentId];
            Quaternion qq = parentJoint->orientation;
            Vector3 pp = qq * position;
            
            position = pp + parentJoint->position;
            orientation = qq * orientation;
        }
         */
        
        //Vector3 position = position0;
        //Quaternion orientation = BuildQuaternion(orientation0.x, orientation0.y, orientation0.z);
        joint->position = position;
        joint->orientation = orientation;
        
        Matrix3 rot;
        orientation.ToRotationMatrix(rot);
        
        joint->transform = Matrix4(rot);
        joint->transform.setTrans(position);
        
        
        joint->transform = transform;
        
        joint->worldTransform = joint->transform;
    }
}


void TGBOModel::Render()
{
    TFlatRenderer* renderer = TFlatRenderer::GetInstance();
    
    
    int maxWeightCount = 0;
    for(int mesh=0;mesh<(int)meshes.size();++mesh)
    {
        
        
        
        for(size_t tri=0;tri<meshes[mesh]->triangles.size();++tri)
        {
            TGBOVertex* vs[3];
            int i0 = meshes[mesh]->triangles[tri].v0;
            int i1 = meshes[mesh]->triangles[tri].v1;
            int i2 = meshes[mesh]->triangles[tri].v2;
            
            assert( i0 < (int)meshes[mesh]->vertices.size());
            assert( i1 < (int)meshes[mesh]->vertices.size());
            assert( i2 < (int)meshes[mesh]->vertices.size());
            
            
            vs[0] = & meshes[mesh]->vertices[i0];
            vs[1] = & meshes[mesh]->vertices[i1];
            vs[2] = & meshes[mesh]->vertices[i2];
            
            Vector3 jps[3];
            jps[0] = Vector3::ZERO;
            jps[1] = Vector3::ZERO;
            jps[2] = Vector3::ZERO;
            
            
#if 0
            // softtware skinning, quaternion way
            for(int v=0;v<3;++v)
            {
                int wCount = vs[v]->weightCount;
                maxWeightCount = wCount > maxWeightCount ? wCount:maxWeightCount;
                //wCount = wCount>=4 ? 4:wCount;
                assert( wCount > 0 );
                for(int w=0;w<wCount;++w)
                {
                    const TMD5Weight& weight = mWegihts[w + vs[v]->startWeight];
                    int jointIndex = weight.mJointIndex;
                    
                    TMD5Joint* joint = mJoints[jointIndex];
                    
                    Vector3 wp(weight.mPosition.x, weight.mPosition.y, weight.mPosition.z);
                    wp = joint->mOrientation * wp + joint->GetPosition();
                    
                    jps[v] += weight.mBias * wp;
                }
            }
#else
            // software skinning, matrix way
            /*
             for(int v=0;v<3;++v)
             {
             int wCount = vs[v]->weightCount;
             
             
             for(int w=0;w<wCount;++w)
             {
             const TMD5Weight& weight = mWegihts[w + vs[v]->startWeight];
             
             
             TMD5Joint* joint = mJoints[weight.mJointIndex];
             
             Vector4 wp(weight.mPosition.x, weight.mPosition.y, weight.mPosition.z, 0.0f);
             
             wp = joint->transform * wp;
             
             Vector3 jp = joint->transform.getTrans();
             jps[v] += weight.mBias * (jp+ Vector3(wp.x, wp.y, wp.z));
             }
             }
             #endif
             */
            
            // suitable to easily move to GPU skinning
            
#if 1
            for(int v=0;v<3;++v)
            {
                int wCount = vs[v]->weightCount;
                Vector4 bvp = Vector4(vs[v]->bindPosePosition.x,
                                      vs[v]->bindPosePosition.y,
                                      vs[v]->bindPosePosition.z, 1.0f);
                
                if( wCount > 0 )
                {
                    float sum = 0.0f;
                    for(int w=0;w<wCount;++w)
                    {
                        const TGBOWeight& weight = meshes[mesh]->weights[w + vs[v]->weightStart];
                        
                        assert(weight.jointIndex < (int)joints.size());
                        TGBOJoint* joint = joints[weight.jointIndex];
                        
                        Vector4 ppp = joint->worldTransform * joint->inverseBindPose * bvp * weight.weight;
                        
                        sum += weight.weight;
                        
                        jps[v] += Vector3(ppp.x, ppp.y, ppp.z);
                        
                    }
                    assert( sum>=0.99f && sum<1.01f);
                }
            }
#endif
#endif
            float u1 = vs[0]->u;
            float v1 = vs[0]->v;
            
            float u2 = vs[1]->u;
            float v2 = vs[1]->v;
            
            float u3 = vs[2]->u;
            float v3 = vs[2]->v;
            
            
            
            TFlatRenderer::TVertexData pt0(jps[0].x, jps[0].y, jps[0].z, u1, v1);
            TFlatRenderer::TVertexData pt1(jps[1].x, jps[1].y, jps[1].z, u2, v2);
            TFlatRenderer::TVertexData pt2(jps[2].x, jps[2].y, jps[2].z, u3, v3);
            
            renderer->RenderTriangle(pt0, pt1, pt2);
            
            
            
            
        }
        
        renderer->Flush();
    }
}

#pragma pack(pop)