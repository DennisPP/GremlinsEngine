#include "ColladaNewParam.h"

#include "ColladaSurface.h"
#include "ColladaSampler2D.h"

#include "tinyxml.h"

TColladaNewParam::TColladaNewParam() : TColladaBase()
{
    surface = NULL;
    sampler2D = NULL;
}

TColladaNewParam::~TColladaNewParam()
{
    if( surface )
    {
        delete surface;
        surface = NULL;
    }
    if( sampler2D )
    {
        delete sampler2D;
        sampler2D = NULL;
    }
}

TColladaBase* TColladaNewParam::Parse(TiXmlElement* xml)
{
    TColladaBase::Parse(xml);
    
    const char* sid = xml->Attribute("sid");
    this->sid = sid ? std::string(sid) : "";
    
    TiXmlElement* surfaceElement = xml->FirstChildElement("surface");
    if( surfaceElement )
    {
        surface = new TColladaSurface();
        surface->Parse(surfaceElement);
    }
    
    TiXmlElement* sampler2DElement = xml->FirstChildElement("sampler2D");
    if( sampler2DElement )
    {
        sampler2D = new TColladaSampler2D();
        sampler2D->Parse(sampler2DElement);
    }   
    
    
    return this;
}