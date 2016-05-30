#include "ColladaEffect.h"

#include "ColladaProfileCommon.h"

#include "tinyxml.h"

TColladaEffect::TColladaEffect() : TColladaBase()
{

}

TColladaEffect::~TColladaEffect()
{
    if( profileCommon )
    {
        delete profileCommon;
        profileCommon = NULL;
    }
}

TColladaBase* TColladaEffect::Parse(TiXmlElement* xml)
{
    TColladaBase::Parse(xml);
    
    const char* name = xml->Attribute("name");
    this->name = name ? std::string(name) : "";
    
    TiXmlElement* profileCommonElement = xml->FirstChildElement("profile_COMMON");
    if( profileCommonElement )
    {
        profileCommon = new TColladaProfileCommon();
        profileCommon->Parse(profileCommonElement);
    }
    
    return this;
}