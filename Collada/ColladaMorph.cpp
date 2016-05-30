#include "ColladaMorph.h"

#include "ColladaSource.h"
#include "ColladaMorphTargets.h"
#include "ColladaInput.h"

#include "tinyxml.h"

TColladaMorph::TColladaMorph()
{

}

TColladaBase* TColladaMorph::Parse(TiXmlElement* xml)
{
    TColladaBase::Parse(xml);
    
    const char* method = xml->Attribute("method");
    const char* source = xml->Attribute("source");
    this->method = method ? std::string(method) : "";
    this->source = source ? std::string(source) : "";
    
    TiXmlElement* sourceElement = xml->FirstChildElement("source");
    for(;sourceElement;sourceElement=sourceElement->NextSiblingElement("source"))
    {
        TColladaSource* source = new TColladaSource();
        source->Parse(sourceElement);
        sources.push_back(source);
    }
    TiXmlElement* targetElement = xml->FirstChildElement("targets");
    for(;targetElement;targetElement=targetElement->NextSiblingElement("targets"))
    {
        TColladaMorphTargets* targets = new TColladaMorphTargets();
        targets->Parse(targetElement);
        morphTargets.push_back(targets);
    }
    
    
    return this;
}

TColladaSource* TColladaMorph::GetSourceById(const std::string& id) const
{
    for(size_t j=0;j<sources.size();++j)
    {
        if( id == sources[j]->id )
            return sources[j];
    }
    return NULL;
}


TColladaSource* TColladaMorph::GetMorphTargetSource() const
{
    for(size_t j=0;j<morphTargets.size();++j)
    {
        TColladaMorphTargets* targets = morphTargets[j];
        for(size_t input=0;input<targets->inputs.size();++input)
        {
            if( targets->inputs[input]->semantic == "MORPH_TARGET" )
            {
                std::string sid = targets->inputs[input]->source;
                TColladaSource* source = GetSourceById(TColladaParserUtils::SkipStartChar(sid));
                if( source )
                    return source;
            }
        }
    }
    
    return NULL;
}

TColladaSource* TColladaMorph::GetMorphWeightSource() const
{
    for(size_t j=0;j<morphTargets.size();++j)
    {
        TColladaMorphTargets* targets = morphTargets[j];
        for(size_t input=0;input<targets->inputs.size();++input)
        {
            if( targets->inputs[input]->semantic == "MORPH_WEIGHT" )
            {
                std::string sid = targets->inputs[input]->source;
                TColladaSource* source = GetSourceById(TColladaParserUtils::SkipStartChar(sid));
                if( source )
                    return source;
            }
        }
    }
    
    return NULL;

}
