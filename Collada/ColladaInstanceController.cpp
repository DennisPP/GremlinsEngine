#include "ColladaInstanceController.h"

#include "ColladaBindMaterial.h"

#include "tinyxml.h"


TColladaInstanceController::TColladaInstanceController() : TColladaBase()
{
    bindMaterial = NULL;
}

TColladaInstanceController::~TColladaInstanceController()
{
    if( bindMaterial )
    {
        delete bindMaterial;
        bindMaterial = NULL;
    }
}


TColladaBase* TColladaInstanceController::Parse(TiXmlElement* xml)
{
    const char* url = xml->Attribute("url");
    this->url = url ? std::string(url) : "";

    TiXmlElement* skeletonElement = xml->FirstChildElement("skeleton");
    for(;skeletonElement;skeletonElement=skeletonElement->NextSiblingElement("skeleton"))
    {
        const char* text = skeletonElement->GetText();
        assert( text );
        skeletons.push_back(std::string(text));
    }
    TiXmlElement* bindMaterialElement = xml->FirstChildElement("bind_material");
    
    if( bindMaterialElement )
    {
        bindMaterial = new TColladaBindMaterial();
        bindMaterial->Parse(bindMaterialElement);
    }
    
    return this;
}