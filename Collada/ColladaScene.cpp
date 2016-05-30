#include "ColladaScene.h"

#include "ColladaInstanceVisualScene.h"


#include "tinyxml.h"


TColladaBase* TColladaScene::Parse(TiXmlElement* xml)
{
    TColladaBase::Parse(xml);
    
    const char* tag = "instance_visual_scene";
    
    TiXmlElement* instanceVisualSceneElement = xml->FirstChildElement(tag);
    for(;instanceVisualSceneElement;instanceVisualSceneElement=instanceVisualSceneElement->NextSiblingElement(tag))
    {
        TColladaInstanceVisualScene* instanceVisualScene = new TColladaInstanceVisualScene();
        instanceVisualScene->Parse(instanceVisualSceneElement);
        instanceVisualScenes.push_back(instanceVisualScene);
    }
    
    return this;
}