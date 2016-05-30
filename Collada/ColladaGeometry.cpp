#include "ColladaGeometry.h"

#include "ColladaMesh.h"

#include "tinyxml.h"


TColladaGeometry::TColladaGeometry() : TColladaBase()
{
    mesh = NULL;
}

TColladaBase* TColladaGeometry::Parse(TiXmlElement* xml)
{
    const char* id = xml->Attribute("id");
    const char* name = xml->Attribute("name");
    
    this->id = id ? std::string(id) : "";
    this->name = name ? std::string(name) : "";
    
    
    TiXmlElement* meshElement = xml->FirstChildElement("mesh");
    if( meshElement )
    {
        this->mesh = new TColladaMesh(this);
        mesh->Parse(meshElement);
    }
    
    return this;
}
