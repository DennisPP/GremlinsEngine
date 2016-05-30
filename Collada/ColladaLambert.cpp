#include "ColladaLambert.h"
#include "ColladaSurface.h"

#include "tinyxml.h"

TColladaLambert::TColladaLambert()
{

}

TColladaLambert::~TColladaLambert()
{
}

TColladaBase* TColladaLambert::Parse(TiXmlElement* xml)
{
       
    TiXmlElement* diffuseElement = xml->FirstChildElement("diffuse");
    if( diffuseElement )
    {
        TiXmlElement* textureElement = diffuseElement->FirstChildElement("texture");
        if( textureElement )
        {
            const char* texture = textureElement->Attribute("texture");
            diffuseTexture = texture ? std::string(texture) : "";
        }
    }

    return this;
}
