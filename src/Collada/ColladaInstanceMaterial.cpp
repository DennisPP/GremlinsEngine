#include "ColladaInstanceMaterial.h"

#include "tinyxml.h"

TColladaBase* TColladaInstanceMaterial::Parse(TiXmlElement* xml)
{
    TColladaBase::Parse(xml);
    const char* symbol = xml->Attribute("symbol");
    const char* target = xml->Attribute("target");
    
    this->symbol = symbol ? std::string(symbol) : "";
    this->target = target ? std::string(target) : "";
    
    return this;
}