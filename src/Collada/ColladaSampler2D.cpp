#include "ColladaSampler2D.h"
#include "tinyxml.h"

TColladaSampler2D::TColladaSampler2D() : TColladaBase()
{
}

TColladaSampler2D::~TColladaSampler2D()
{
}

TColladaBase* TColladaSampler2D::Parse(TiXmlElement* xml)
{
    TColladaBase::Parse(xml);
    
    TiXmlElement* sourceElement = xml->FirstChildElement("source");
    if( sourceElement )
    {
        const char* s = sourceElement->GetText();
        source = s ? std::string(s) : "";
    }

    return this;
}
