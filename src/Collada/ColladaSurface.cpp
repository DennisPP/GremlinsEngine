#include "ColladaSurface.h"

#include "tinyxml.h"

TColladaSurface::TColladaSurface() : TColladaBase()
{
}

TColladaSurface::~TColladaSurface()
{
}

TColladaBase* TColladaSurface::Parse(TiXmlElement* xml)
{
    TColladaBase::Parse(xml);
    
    const char* type = xml->Attribute("type");
    this->type = type ? std::string(type) : "";
    
    TiXmlElement* initFromElement = xml->FirstChildElement("init_from");
    if( initFromElement )
    {
        const char* from = initFromElement->GetText();
        initFrom = from ? std::string(from) : "";
    }    

    return this;
}