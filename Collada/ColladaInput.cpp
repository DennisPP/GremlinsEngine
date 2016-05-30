#include "ColladaInput.h"


#include "tinyxml.h"

TColladaBase* TColladaInput::Parse(TiXmlElement* xml)
{
    const char* semantic = xml->Attribute("semantic");
    const char* source = xml->Attribute("source");
    xml->Attribute("offset", &offset);
    
    if( semantic )
        this->semantic = std::string(semantic);
    if( source )
        this->source = std::string(source);
    
    return this;
}