//
//  ColladaSource.cpp
//  ModelViewer
//
//  Created by dennis on 12/3/2015.
//  Copyright (c) 2015年 dennis. All rights reserved.
//

#include "ColladaSource.h"

#include "tinyxml.h"

TColladaIDREFArray::TColladaIDREFArray()
{
    count = 0;
}

TColladaBase* TColladaIDREFArray::Parse(TiXmlElement* xml)
{
    TColladaBase::Parse(xml);
    const char* name = xml->Attribute("name");
    this->name = name ? std::string(name) : "";
    
    xml->Attribute("count", &count);
    
    assert( xml );
    const char* text = xml->GetText();
    if( text )
        TColladaParserUtils::ParseNameArray(text, ids);
    return this;
}


TColladaBase* TColladaNameArray::Parse(TiXmlElement* xml)
{
    assert( xml );
    
    const char* text = xml->GetText();
    if( text )
        TColladaParserUtils::ParseNameArray(text, names);
    
    return this;
}

int TColladaAccessor::GetParamOffset(TColladaParam* p) const
{
    for(int j=0;j<(int)params.size();++j)
        if( p== params[j])
            return j;
    return -1;
}


TColladaSource::TColladaSource() : TColladaBase()
{
    nameArray = NULL;
    idRefArray = NULL;
    stride = 1;
}


TColladaBase* TColladaParam::Parse(TiXmlElement *xml)
{
    const char* name = xml->Attribute("name");
    const char* type = xml->Attribute("type");
    if( name )
        this->name = std::string(name);
    if( type )
        this->type = std::string(type);
    
    return this;
}

TColladaBase* TColladaAccessor::Parse(TiXmlElement* xml)
{
    const char* source = xml->Attribute("source");
    xml->Attribute("count", &count);
    xml->Attribute("stride", &stride);
    int offset = 0;
    xml->Attribute("offset", &offset);
    
    this->offset = offset;
    
    if( source )
        this->source = std::string(source);
    
    TiXmlElement* paramElement = xml->FirstChildElement("param");
    for(;paramElement;paramElement=paramElement->NextSiblingElement("param"))
    {
        TColladaParam* param = new TColladaParam();
        param->Parse(paramElement);
        
        params.push_back(param);
    }
    
    return this;
}


TColladaParam* TColladaSource::GetParamByName(const std::string& name) const
{
    for(size_t j=0;j<accessor->params.size();++j)
    {
        TColladaParam* p = accessor->params[j];
        if( p->name == name )
            return p;
    }
    return NULL;
}


TColladaBase* TColladaSource::Parse(TiXmlElement* sourceElement)
{
    const char* id = sourceElement->Attribute("id");
    assert( id );
    const char* name = sourceElement->Attribute("name");
    
    
    
    
    TiXmlElement* floatArrayElement = sourceElement->FirstChildElement("float_array");
    TiXmlElement* nameArrayElement  = sourceElement->FirstChildElement("Name_array");
    TiXmlElement* IDREFArrayElement = sourceElement->FirstChildElement("IDREF_array");
    if( nameArrayElement )
    {
        nameArray = new TColladaNameArray();
        nameArray->Parse(nameArrayElement);
    }
    if( IDREFArrayElement )
    {
        idRefArray = new TColladaIDREFArray();
        idRefArray->Parse(IDREFArrayElement);
    }
    
    
    
    this->id = std::string(id);
    
    if( name )
        this->name = std::string(name);
    
    if( floatArrayElement )
    {
        const char* text = floatArrayElement->GetText();
        TColladaParserUtils::ParseFloatArray(text, values);
    }
    
    TiXmlElement* technique_common = sourceElement->FirstChildElement("technique_common");
    if( technique_common )
    {
        TiXmlElement* accessorElement = technique_common->FirstChildElement("accessor");
        if( accessorElement )
        {
            TColladaAccessor* accessor = new TColladaAccessor();
            accessor->Parse(accessorElement);
            this->stride = accessor->stride;
            
            this->accessor = accessor;
        }
    }
    
    return this;
}


