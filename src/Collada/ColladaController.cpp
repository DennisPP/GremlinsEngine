#include "ColladaController.h"

#include "ColladaSkin.h"
#include "ColladaMorph.h"

#include "tinyxml.h"

TColladaController::TColladaController()
{
    skin = NULL;
    morph = NULL;
}


TColladaController::~TColladaController()
{
    if( skin )
    {
        delete skin;
        skin = NULL;
    }
    if( morph )
    {
        delete morph;
        morph = NULL;
    }
}


TColladaBase* TColladaController::Parse(TiXmlElement* xml)
{
    const char* id = xml->Attribute("id");
    assert( id );
    
    this->id = std::string(id);
    
    TiXmlElement* skinElement = xml->FirstChildElement("skin");
    if( skinElement )
    {
        skin = new TColladaSkin();
        skin->Parse(skinElement);
    }
    TiXmlElement* morphElement = xml->FirstChildElement("morph");
    if( morphElement )
    {
        morph = new TColladaMorph();
        morph->Parse(morphElement);
    }
    
    return this;
}
