#include "ColladaImage.h"

#include "tinyxml.h"

TColladaImage::TColladaImage() : TColladaBase()
{
}

TColladaBase* TColladaImage::Parse(TiXmlElement* xml)
{
    TColladaBase::Parse(xml);
    
    const char* name = xml->Attribute("name");
    this->name = name ? std::string(name) : "";
    
    TiXmlElement* initFrom = xml->FirstChildElement("init_from");
    if( initFrom )
        path = std::string(initFrom->GetText());
    return this;
}

