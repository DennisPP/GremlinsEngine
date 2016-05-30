#include "ColladaMaterial.h"

#include "ColladaInstanceEffect.h"

#include "tinyxml.h"


TColladaMaterial::TColladaMaterial(): TColladaBase()
{
}

TColladaMaterial::~TColladaMaterial()
{
    for(size_t j=0;j<instanceEffects.size();++j)
    {
        if( instanceEffects[j] )
            delete instanceEffects[j];
    }
    instanceEffects.clear();
}

TColladaBase* TColladaMaterial::Parse(TiXmlElement* xml)
{    
    TColladaBase::Parse(xml);
    
    
    const char* materialName = xml->Attribute("name");
    name = materialName ? std::string(materialName) : "";
        
    
    TiXmlElement* instanceEffectElement = xml->FirstChildElement("instance_effect");
    for(;instanceEffectElement;instanceEffectElement=instanceEffectElement->NextSiblingElement("instance_effect"))
    {
        TColladaInstanceEffect* instanceEffect = new TColladaInstanceEffect();
        instanceEffect->Parse(instanceEffectElement);
        instanceEffects.push_back(instanceEffect);
    }
    return this;
}