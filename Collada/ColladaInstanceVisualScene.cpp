#include "ColladaInstanceVisualScene.h"

#include "tinyxml.h"

TColladaBase* TColladaInstanceVisualScene::Parse(TiXmlElement* xml)
{
    TColladaBase::Parse(xml);
    const char* sid = xml->Attribute("sid");
    const char* url = xml->Attribute("url");
    const char* name = xml->Attribute("name");

    this->sid = sid ? std::string(sid) : "";
    this->url = url ? std::string(url) : "";
    this->name = name ? std::string(name) : "";
    
    return this;
}
