#include "ColladaBindMaterial.h"

#include "ColladaInstanceMaterial.h"

#include "tinyxml.h"

TColladaBindMaterial::TColladaBindMaterial() : TColladaBase()
{
    instanceMaterial = NULL;
}

TColladaBindMaterial::~TColladaBindMaterial()
{
    if( instanceMaterial )
    {
        delete instanceMaterial;
        instanceMaterial = NULL;
    }
}

TColladaBase* TColladaBindMaterial::Parse(TiXmlElement* xml)
{
    TColladaBase::Parse(xml);
    
    TiXmlElement* techniqueCommon = xml->FirstChildElement("technique_common");

    if( techniqueCommon )
    {
        TiXmlElement* instanceMaterialElement = techniqueCommon->FirstChildElement("instance_material");
        instanceMaterial = new TColladaInstanceMaterial();
        instanceMaterial->Parse(instanceMaterialElement);
    }
    return this;
}