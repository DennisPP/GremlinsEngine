#include "ColladaTriangle.h"

#include "ColladaInput.h"
#include "tinyxml.h"


bool TColladaTriangle::UseTextureIndex() const
{
    for(size_t j=0;j<inputs.size();++j)
    {
        if( inputs[j]->semantic == std::string("TEXCOORD") )
            return true;
    }
    return false;
}

TColladaBase* TColladaTriangle::Parse(TiXmlElement* xml)
{
    xml->Attribute("count", &triangleCount);
    const char* mat = xml->Attribute("material");
    material = mat ? std::string(mat) : "";
    
    TiXmlElement* inputElement = xml->FirstChildElement("input");
    for(;inputElement;inputElement=inputElement->NextSiblingElement("input"))
    {
        TColladaInput* input = new TColladaInput();
        input->Parse(inputElement);
        inputs.push_back(input);
        const std::string& semantic = input->semantic;
        
        if( semantic=="VERTEX")
        {
            vertexSource = input->source;
        }
        else if( semantic=="TEXCOORD")
        {
            texCoordSource = input->source;
        }
        else if( semantic=="NORMAL")
        {
            normalSource = input->source;
        }
    }
    TiXmlElement* p = xml->FirstChildElement("p");
    assert( p );
    TColladaParserUtils::ParseIntArray(p->GetText(), this->p);
    
    return this;
}

