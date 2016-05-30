#include "ColladaModel.h"


#include "ColladaInput.h"

#include "ColladaImage.h"
#include "ColladaSkin.h"
#include "ColladaNode.h"
#include "ColladaMesh.h"
#include "ColladaGeometry.h"
#include "ColladaMaterial.h"
#include "ColladaAsset.h"
#include "ColladaController.h"
#include "ColladaVisualScene.h"
#include "ColladaScene.h"
#include "ColladaEffect.h"

#include "Graphics/Graphics.h"

#include <queue>
#include <stack>
#include <map>

#include "tinyxml.h"


#include "Math3D/Math3D.h"

#include <string>

#include <vector>

bool IsEndWith(const char* string, const char* end)
{
    const char* ptr0 = string + strlen(string);
    const char* ptr1 = end + strlen(end);
    for(;;)
    {
        if( *ptr0 != *ptr1 )
            return false;
        if( ptr1==end )
            return true;
        if( ptr0==string)
            return false;
        --ptr0;
        --ptr1;
    }
    return true;
}

TColladaSource* TColladaModel::FindSourceById(const std::string& id)
{
    assert( ! geometrys.empty() );
    TColladaMesh* mesh = geometrys[0]->mesh;

    for(size_t j=0;j<mesh->sources.size();++j)
    {
        if( mesh->sources[j]->id == id )
            return mesh->sources[j];
    }
    printf("%s:Source NOT FOUND!\n", id.c_str());
    return NULL;
}

TColladaModel::TColladaModel()
{
    mPlayedTime = 0.0f;
}

TColladaModel::~TColladaModel()
{
    if( scene )
    {
        delete scene;
        scene = NULL;
    }
}

TColladaAsset* TColladaModel::GetAsset() const
{
    return asset;
}


TColladaSource* TColladaModel::FindVertexSourceById(const std::string& id)
{
    assert( ! geometrys.empty() );
    TColladaMesh* mesh = geometrys[0]->mesh;

    for(size_t j=0;j<mesh->vertices.size();++j)
    {
        if( id == mesh->vertices[j]->id )
        {
            // skip the #
            size_t length = mesh->vertices[j]->source.length();
            std::string s = mesh->vertices[j]->source.substr(1, length-1);
            return FindSourceById(s);
        }
    }
    return NULL;
}

TColladaVisualScene* TColladaModel::FindVisualSceneById(const std::string& id) const
{
    for(size_t j=0;j<visualScenes.size();++j)
    {
        if( visualScenes[j]->id == id )
            return visualScenes[j];
    
    }
    return NULL;
}


TColladaController* TColladaModel::FindControllerById(const std::string& id) const
{
    for(size_t c=0;c<controllers.size();++c)
    {
        if( controllers[c]->id == id )
            return controllers[c];
    }
    return NULL;
}

TColladaMaterial* TColladaModel::FindMaterialById(const std::string& id) const
{
    for(size_t c=0;c<materials.size();++c)
    {
        if( materials[c]->id == id )
            return materials[c];
    }
    return NULL;
}


TColladaEffect* TColladaModel::FindEffectById(const std::string& id) const
{
    for(size_t c=0;c<effects.size();++c)
    {
        if( effects[c]->id == id )
            return effects[c];
    }
    return NULL;
}


TColladaImage* TColladaModel::FindImageById(const std::string& id) const
{
    for(size_t c=0;c<images.size();++c)
    {
        if( images[c]->id == id )
            return images[c];
    }
    return NULL;


}

std::string TColladaModel::SkipStartChar(const std::string& s)
{
    size_t length = s.length();
    return s.substr(1, length-1);
}







void TColladaModel::LoadModel(const char* filename)
{
    loadingPath = std::string(filename);
    
    nodeId = 0;
    
    
    TiXmlDocument doc;
    bool loaded = doc.LoadFile(filename);
    assert( loaded );
    
    
    TiXmlElement* rootElement = doc.FirstChildElement();
    
    TiXmlElement* imageLibrary = rootElement->FirstChildElement("library_images");
    TiXmlElement* controllerLibrary = rootElement->FirstChildElement("library_controllers");
    TiXmlElement* visualSceneLibrary = rootElement->FirstChildElement("library_visual_scenes");
    TiXmlElement* animationLibrary = rootElement->FirstChildElement("library_animations");
    TiXmlElement* materialLibrary = rootElement->FirstChildElement("library_materials");
    TiXmlElement* geoLibrary = rootElement->FirstChildElement("library_geometries");
    TiXmlElement* effectLibrary = rootElement->FirstChildElement("library_effects");
    
    
    TiXmlElement* assetElement = rootElement->FirstChildElement("asset");
    if( assetElement )
    {
        asset = new TColladaAsset();
        asset->Parse(assetElement);
    }
    
    if( imageLibrary )
    {
        TiXmlElement* imageElement = imageLibrary->FirstChildElement("image");
        for(;imageElement;imageElement=imageElement->NextSiblingElement("image"))
        {
            TColladaImage* image = new TColladaImage();
            image->Parse(imageElement);
            images.push_back(image);
        }
    }
    
    if( materialLibrary )
    {
        TiXmlElement* materialElement = materialLibrary->FirstChildElement("material");
        for(;materialElement;materialElement=materialElement->NextSiblingElement("material"))
        {
            TColladaMaterial* material = new TColladaMaterial();
            material->Parse(materialElement);
            materials.push_back(material);
        }
    }
    
    if( geoLibrary)
    {
        TiXmlElement* geometryElement = geoLibrary->FirstChildElement("geometry");
        for(;geometryElement;geometryElement=geometryElement->NextSiblingElement("geometry") )
        {
            TColladaGeometry* geometry = new TColladaGeometry();
            geometry->Parse(geometryElement);
            geometrys.push_back(geometry);
        }
    }
    
    if( effectLibrary )
    {
        TiXmlElement* effectElement = effectLibrary->FirstChildElement("effect");
        for(;effectElement;effectElement=effectElement->NextSiblingElement("effect"))
        {
            TColladaEffect* effect = new TColladaEffect();
            effect->Parse(effectElement);
            effects.push_back(effect);
        }
    }
    
    
    if( controllerLibrary )
    {
        TiXmlElement* controllerElement = controllerLibrary->FirstChildElement("controller");
        for(;controllerElement;controllerElement=controllerElement->NextSiblingElement("controller") )
        {
            TColladaController* controller = new TColladaController();
            controller->Parse(controllerElement);
            controllers.push_back(controller);
        }
    }
    
    if( visualSceneLibrary )
    {
        TiXmlElement* visualSceneElement = visualSceneLibrary->FirstChildElement("visual_scene");
        for(;visualSceneElement;visualSceneElement=visualSceneElement->NextSiblingElement("visual_scene"))
        {
            TColladaVisualScene* visualScene = new TColladaVisualScene();
            visualScene->Parse(visualSceneElement);
            visualScenes.push_back(visualScene);
        }
    }
    
    if( animationLibrary )
    {
        TiXmlElement* animation = animationLibrary->FirstChildElement("animation");
        for(;animation;animation=animation->NextSiblingElement("animation"))
        {
            const char* id = animation->Attribute("id");
            TColladaAnimation* anim = new TColladaAnimation();
            anim->Parse(animation);
            if( id )
                anim->id = std::string(id);
            animations.push_back(anim);
        }
    }
    
    
    TiXmlElement* sceneElement = rootElement->FirstChildElement("scene");
    if( sceneElement )
    {
        scene = new TColladaScene();
        scene->Parse(sceneElement);
    }
    
    
    
    printf("***************************************\n");
    printf("Source count:%d\n", (int)sources.size());
    printf("Joint Node count:%d\n", (int)jointNodes.size());
    //printf("Node cound:%d\n", (int)nodes.size());
    printf("Animation count:%d\n", (int)animations.size());
    printf("***************************************\n");
    
    

    assert( visualScenes.size() == 1 );
    TColladaVisualScene* visualScene = visualScenes[0];

    
    
    #if 1
    std::map<std::string, int> counter;
    
    // set the target node for all animations
    
    std::vector<TColladaAnimation*> collectedAnimations;
    CollectAnimations(collectedAnimations);
    for(int j=0;j<collectedAnimations.size();++j)
    {
        TColladaAnimation* animation = collectedAnimations[j];
        assert( animation->channels.size() <= 1 );
        for(int c=0;c<animation->channels.size();++c)
        {
            TColladaChannel* chan = animation->channels[c];
        
            std::string target = chan->target;
            std::string targetChannel = chan->target;
            size_t p = target.find_first_of("/");
            if( p != std::string::npos )
            {
                targetChannel = target.substr(1+p);
                target = target.substr(0, p);
            }

            TColladaNode* targetNode = visualScene->FindNodeById(target);
            if( targetNode )
            {
                animation->SetTargetNode(targetNode);
            }
        }
        
        std::map<std::string, int>::iterator it = counter.find(animation->target);
        if( it == counter.end())
        {
            counter[animation->target] = 1;
        }
        else
        {
            counter[animation->target] ++;
        }
        printf("Animation:%s\n",   animation->target.c_str());
    }
    #endif
    
    std::map<std::string, int>::iterator it = counter.begin();
    for(;it!=counter.end();++it)
    {
        if( it->second > 1)
        {
            printf("WARNINGS:::::Duplicate animation target:%s:%d\n", it->first.c_str(), it->second);
        }
    }
    
    
    
    
#if 0
    int jointCount = 0;
    int treeCount = 0;
    for(size_t j=0;j<nodes.size();++j)
    {
        if( nodes[j]->IsRootNode())
        {
            nodes[j]->Dump();
            
            
            if( nodes[j]->IsValid() )
            {
                printf("Joint Tree %d IS VALID!!!\n", treeCount);
            }
            
            
            
            ++ treeCount;
        }
        if( nodes[j]->type == "JOINT" )
            ++jointCount;
    }
    
    printf("JOINTS:%d\n", jointCount);
    printf("Trees:%d\n", treeCount);
#endif
    
    
    
    std::string basePath = "";
    size_t pos = loadingPath.find_last_of("/");
    if( pos != std::string::npos )
    {
        basePath = loadingPath.substr(0, pos);
    }
    
    #if 1
    for(int j=0;j<(int)images.size();++j)
    {
        std::string path = basePath + "/" + images[j]->path;
        if( TTextureManager::IsExist(path.c_str()) )
        {
            unsigned int texture = TTextureManager::GetInstance().GetTexture(path.c_str()).TextureID;
            textures[images[j]] = texture;
        }
    }
    #endif
    
    
    std::vector<TColladaInstanceController*> controllers;
    CollectInstanceController(controllers);
    
    int a = 0;
}


TColladaGeometry* TColladaModel::FindGeometryById(const std::string& id) const
{
    for(size_t j=0;j<geometrys.size();++j)
    {
        if( id == geometrys[j]->id )
            return geometrys[j];
    }
    return NULL;
}


void TColladaModel::Update(float dt)
{
    int treeCount = 0;
    #if 0
    for(size_t s=0;s<visualScenes.size();++s)
    {
        TColladaVisualScene* scene = visualScenes[s];
        for(size_t n=0;n<scene->nodes.size();++n)
        {
            TColladaNode* node = scene->nodes[n];
            if( node->IsRootNode() )
            {
                node->UpdateTransform();
                ++ treeCount;
            }
        }
    }
    #endif

    for(size_t s=0;s<visualScenes.size();++s)
    {
        TColladaVisualScene* scene = visualScenes[s];
        for(size_t n=0;n<scene->rootNodes.size();++n)
        {
            TColladaNode* node = scene->rootNodes[n];
            assert( node->IsRootNode() );
            node->UpdateTransform();
            ++ treeCount;
        }
    }
    
    
    TColladaModel* colladaModel = this;
    std::vector<TColladaAnimation*> animations;
    colladaModel->CollectAnimations(animations);
    
    if( animations.empty())
        return;

    //TColladaVisualScene* visualScene = colladaModel->visualScenes.empty() ? NULL : colladaModel->visualScenes[0];

    // HACK use the first valid animation to find the frame count and total time
    //
    int keyFrameCount = 0;
    float totalTime = 0.0f;
    for(size_t anim=0;anim<animations.size();++anim)
    {
        TColladaAnimation* animation = animations[anim];
        TColladaSource* timeSource = animation->GetTimeSource();
        if( timeSource )
        {
            keyFrameCount = (int)timeSource->values.size();
            totalTime = timeSource->values[keyFrameCount-1];
            break;
        }
    }
    assert( keyFrameCount != 0 );

    
    mPlayedTime += dt;
    if( mPlayedTime > totalTime )
    mPlayedTime = fmod(mPlayedTime, totalTime);
   


    std::map<TColladaNode*, int> nodeMap;
    std::map<TColladaNode*, Matrix4> nodeTransform;
    std::map<TColladaNode*, std::vector<std::string> > nodeTargets;
    for(int j=0;j<(int)animations.size();++j)
    {
        TColladaAnimation* animation = animations[j];
        
        int frame = 0;
        
        TColladaSource* timeSource = animation->GetTimeSource();
        if( ! timeSource )
            continue;
        
        int keyFrameCount = (int)timeSource->values.size();
        if( keyFrameCount <= 0 )
            continue;
        
        float totalTime = timeSource->values[keyFrameCount-1];
        
        animation->playedTime += dt;
        if( animation->playedTime  > totalTime )
            animation->playedTime  = fmod(animation->playedTime , totalTime);
        
        
        //mPlayedTime = 0.0f; //HACK:
        
        for(size_t t=0;t<timeSource->values.size();++t)
        {
            if( timeSource->values[t] >= animation->playedTime )
            {
                frame = (int)t;
                break;
            }
        }
        assert( frame>=0 );

        
        
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
            
            TColladaNode* node = animation->targetNode;
            assert( node );
            
            float time = 0.0f;
            Matrix4 transform = Matrix4::IDENTITY;
            
            Quaternion rotation = Quaternion::IDENTITY;
            
            assert( inputSource && outputSource );
            
            
            assert( inputSource->accessor->params.size() == 1 );
            
            if( inputSource->accessor->params[0]->name == "TIME" )
                time = inputSource->values[frame];
                
                
                int stride = outputSource->accessor->stride;
                std::vector<float>& values = outputSource->values;
                
                for(int p=0;p<outputSource->accessor->params.size();++p)
                {
                    TColladaParam* param = outputSource->accessor->params[p];
                    if( param->name == "TRANSFORM" || targetChannel == "matrix" )
                    {
                        //if( param->type == "float4x4" )
                        {
                            std::vector<float>& values = outputSource->values;
                            int startIndex = 16 * frame;
                            
                            Matrix4 mat = TColladaParserUtils::FloatArrayToMatrix(values, startIndex);
                            transform = transform * mat;
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
                        int i = frame;
                        //Quaternion qq0;
                        //qq0.FromAngleAxis(values[i], Vector3::UNIT_Z);
                        //rotation = rotation * qq0;
                    }
                    else if( targetChannel == std::string("blendParent1") )
                    {
                        int a = 0;
                    }
                    else
                    {
                        int b = 0;
                    }
                }
            
            nodeTargets[node].push_back(chan->target);
            if( nodeTransform.find(node)==nodeTransform.end())
            {
                nodeTransform[node] = transform;
            }
            else
            {
                nodeTransform[node] = transform;// * nodeTransform[node]; //HACK
            }
            if( nodeMap.find(node)==nodeMap.end() )
            {
                nodeMap[node] = 1;
            }
            else
            {
                ++ nodeMap[node];
            }
            
        }
    }

    std::map<TColladaNode*, Matrix4>::iterator it = nodeTransform.begin();
    for(;it!=nodeTransform.end();++it)
    {
        TColladaNode* node = it->first;
        const Matrix4& transform = it->second;
        node->transform = node->baseTransform * transform;
        node->worldTransform = node->baseTransform * transform;
        node->transform = transform;
        node->worldTransform = transform;
    }

    #if 0

    std::map<TColladaNode*, std::vector<std::string>>::iterator iit = nodeTargets.begin();

    printf("****************************************************************\n");

    for(;iit!=nodeTargets.end();++iit)
    {
        printf("Node:%s:\n", iit->first->id.c_str());
        
        int count = (int)iit->second.size();
        for(int j=0;j<count;++j)
        {
            printf("--------%s\n", iit->second[j].c_str());
        }
    }
    #endif
    
}


float TColladaModel::GetMaxAnimationTime() const
{
    float maxTime = -1.0f;
    for(int anim=0;anim<(int)animations.size();++anim)
    {
        TColladaSource* timeSource = animations[anim]->GetTimeSource();
        if( timeSource )
        {
            int lastTime = (int)timeSource->values.size()-1;
            assert( lastTime >= 0);
            if( timeSource->values[lastTime] > maxTime)
                maxTime = timeSource->values[lastTime];
        }
    }
    return maxTime;
}


bool TColladaModel::GetInverseBindPoseByJointName(const std::string& name, Matrix4& invBindPose)
{
    Matrix4 result = Matrix4::IDENTITY.inverse();
    int count = 0;
    std::vector<Matrix4> list;
    for(int con=0;con<(int)controllers.size();++con)
    {
        TColladaSkin* skin = controllers[con]->skin;
        TColladaSource* nameSource = skin->GetJointNameSource();
        if( nameSource )
        {
            int jCount =  (int)nameSource->nameArray->names.size();
            for(int j=0;j<jCount;++j)
            {
                const std::string& skinJointName = nameSource->nameArray->names[j];
                if( skinJointName == name)
                {
                    result = skin->invBindPoses[j];
                    list.push_back(skin->invBindPoses[j]);
                    ++ count;
                }
            }
        }
    }
    //assert( count >= 1 );
    if( count == 0 )
    {
        printf("NO Inverse Bind Pose Found!!!!:%s\n", name.c_str());
        return false;
    }
    if( count != 1 )
    {
        for(int j=1;j<(int)list.size();++j)
        {
            assert( list[j-1] == list[j] );
        }
        invBindPose = result;
    }
    invBindPose = result;
    return true;
}


void TColladaModel::CollectInstanceController(std::vector<TColladaInstanceController*>& controllers) const
{
    for(int j=0;j<(int)visualScenes.size();++j)
    {
        TColladaVisualScene* s = visualScenes[j];
        for(int node=0;node<(int)s->allNodes.size();++node)
        {
            TColladaNode* n = s->allNodes[node];
            if( ! n->instanceControllers.empty() )
                controllers.insert(controllers.end(),n->instanceControllers.begin(), n->instanceControllers.end());
        }
    }
}

void TColladaModel::CollectInstanceGeometry(std::vector<TColladaInstanceGeometry*>& gemotrys) const
{
    for(int j=0;j<(int)visualScenes.size();++j)
    {
        TColladaVisualScene* s = visualScenes[j];
        for(int node=0;node<(int)s->allNodes.size();++node)
        {
            TColladaNode* n = s->allNodes[node];
            if( ! n->instanceGeometrys.empty() )
                gemotrys.insert(gemotrys.end(),n->instanceGeometrys.begin(), n->instanceGeometrys.end());
        }
    }
}

void TColladaModel::CollectAnimations(std::vector<TColladaAnimation*>& collectedAnimations) const
{
    for(size_t anim=0;anim<animations.size();++anim)
    {
        std::stack<TColladaAnimation*> st;
        st.push(animations[anim]);
        while( !st.empty())
        {
            TColladaAnimation* animation = st.top();
            st.pop();
            collectedAnimations.push_back(animation);
            for(size_t ch=0;ch<animation->childAnimations.size();++ch)
            {
                st.push(animation->childAnimations[ch]);
            }
        }
    }
}


