#include "ColladaTechnique.h"
#include "ColladaLambert.h"
#include "ColladaPhong.h"

#include "tinyxml.h"

TColladaTechnique::TColladaTechnique()
{
    lambert = NULL;
    phong = NULL;
}

TColladaTechnique::~TColladaTechnique()
{
    if( lambert )
    {
        delete lambert;
        lambert = NULL;
    }
    if( phong )
    {
        delete phong;
        phong = NULL;
    }
}

TColladaBase* TColladaTechnique::Parse(TiXmlElement* xml)
{
    TColladaBase::Parse(xml);
   
    TiXmlElement* lambertElement = xml->FirstChildElement("lambert");
    
    if( lambertElement )
    {
        lambert = new TColladaLambert();
        lambert->Parse(lambertElement);
    }
    
    TiXmlElement* phongElement = xml->FirstChildElement("phong");
    if( phongElement )
    {
        phong = new TColladaPhong();
        phong->Parse(phongElement);
    }
    
    return this;
}