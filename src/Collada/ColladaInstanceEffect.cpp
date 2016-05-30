#include "ColladaInstanceEffect.h"

#include "tinyxml.h"

TColladaInstanceEffect::TColladaInstanceEffect()
{
    
}

TColladaInstanceEffect::~TColladaInstanceEffect()
{
}

TColladaBase* TColladaInstanceEffect::Parse(TiXmlElement* xml)
{
    TColladaBase::Parse(xml);
    
    const char* url = xml->Attribute("url");
    this->url = url ? std::string(url) : "";

    return this;
}
