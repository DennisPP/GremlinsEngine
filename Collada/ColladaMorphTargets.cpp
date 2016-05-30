#include "ColladaMorphTargets.h"
#include "ColladaInput.h"

#include "tinyxml.h"

TColladaBase* TColladaMorphTargets::Parse(TiXmlElement* xml)
{
    TColladaBase::Parse(xml);
    
    TiXmlElement* inputElement = xml->FirstChildElement("input");
    for(;inputElement;inputElement=inputElement->NextSiblingElement("input"))
    {
        TColladaInput* input = new TColladaInput();
        input->Parse(inputElement);
        inputs.push_back(input);
    }
    return this;
}
