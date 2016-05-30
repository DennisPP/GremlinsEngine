#include "ColladaInstanceGeometry.h"

#include "ColladaBindMaterial.h"

#include "tinyxml.h"



TColladaInstanceGeometry::TColladaInstanceGeometry(TColladaNode* _parentNode)
{
    parentNode = _parentNode;
}

TColladaBase* TColladaInstanceGeometry::Parse(TiXmlElement* xml)
{
    const char* url = xml->Attribute("url");
    const char* name = xml->Attribute("name");
    const char* sid = xml->Attribute("sid");
    this->url = url ? std::string(url) : "";
    this->name = name ? std::string(name) : "";
    this->sid = sid ? std::string(sid) : "";
    
    
    TiXmlElement* bindMaterialElement = xml->FirstChildElement("bind_material");
    if( bindMaterialElement )
    {
        bindMaterial = new TColladaBindMaterial();
        bindMaterial->Parse(bindMaterialElement);
    }    
    
    return this;
}