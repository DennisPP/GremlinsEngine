#include "ColladaProfileCommon.h"

#include "ColladaTechnique.h"
#include "ColladaNewParam.h"

#include "tinyxml.h"



TColladaProfileCommon::TColladaProfileCommon() : TColladaBase()
{

}

TColladaProfileCommon::~TColladaProfileCommon()
{
    for(int j=0;j<(int)newParams.size();++j)
        if( newParams[j] )
            delete newParams[j];
    newParams.clear();

}

TColladaBase* TColladaProfileCommon::Parse(TiXmlElement* xml)
{
    TColladaBase::Parse(xml);
    
    TColladaBase::Parse(xml);
    TiXmlElement* newParamElement = xml->FirstChildElement("newparam");
    for(;newParamElement;newParamElement=newParamElement->NextSiblingElement("newparam"))
    {
        TColladaNewParam* newParam = new TColladaNewParam();
        newParam->Parse(newParamElement);
        newParams.push_back(newParam);
    }

    
    TiXmlElement* techniqueElement = xml->FirstChildElement("technique");
    if( techniqueElement )
    {
        technique = new TColladaTechnique();
        technique->Parse(techniqueElement);
    }    

    return this;
}