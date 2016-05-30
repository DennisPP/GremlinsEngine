//
//  ColladaBase.cpp
//  ModelViewer
//
//  Created by dennis on 12/3/2015.
//  Copyright (c) 2015年 dennis. All rights reserved.
//

#include "ColladaBase.h"


#include "Math3D/Vector3.h"
#include "Math3D/Vector4.h"
#include "Math3D/Matrix4.h"

#include "ColladaSource.h"

#include "tinyxml.h"

TColladaBase::TColladaBase()
{
}

TColladaBase::~TColladaBase()
{

}

TColladaBase* TColladaBase::Parse(TiXmlElement* xml)
{
    const char* id = xml->Attribute("id");
    this->id = id ? std::string(id) : "";
    return this;
}

void TColladaParserUtils::ParseFloatArray(const char* text, std::vector<float>& values)
{
    if( ! text )
        return;
    
    char buf[256];
    memset(buf, 0, 256);
    int read;
    const char space = ' ';
    while( *text == space)
        ++text;
    
    while( EOF != (read=sscanf(text, "%s", buf)) )
    {
        float value = atof(buf);
        text += strlen(buf);
        
        values.push_back(value);
        
        
        memset(buf, 0, 256);
        while( *text == space)
            ++text;
    }
}

void TColladaParserUtils::ParseIntArray(const char* text, std::vector<int>& values)
{
    if( ! text )
        return;
    
    char buf[256];
    memset(buf, 0, 256);
    int read;
    const char space = ' ';
    while( *text == space)
        ++text;
    
    while( EOF != (read=sscanf(text, "%s", buf)) )
    {
        float value = atoi(buf);
        text += strlen(buf);
        
        values.push_back(value);
        memset(buf, 0, 256);
        while( *text == space)
            ++text;
    }
}



void TColladaParserUtils::ParseVector3(const char* text, Vector3& v)
{
    float x, y, z;
    sscanf(text, "%f %f %f", &x, &y, &z);
    printf("x:%f y:%f z:%f\n", x, y, z);
    v = Vector3(x, y, z);
}

void TColladaParserUtils::ParseVector4(const char* text, Vector4& v)
{
    float x, y, z, w;
    sscanf(text, "%f %f %f %f", &x, &y, &z, &w);
    printf("x:%f y:%f z:%f w:%f\n", x, y, z, w);
    v = Vector4(x, y, z, w);
}


void TColladaParserUtils::ParseNameArray(const char* text, std::vector<std::string>& names)
{
    if( ! text )
        return;
    
    char buf[256];
    memset(buf, 0, 256);
    int read;
    const char space = ' ';
    while( *text == space)
        ++text;
    
    while( EOF != (read=sscanf(text, "%s", buf)) )
    {
        text += strlen(buf);
        
        names.push_back(std::string(buf));
        
        memset(buf, 0, 256);
        while( *text == space)
            ++text;
    }


}


Matrix4 TColladaParserUtils::FloatArrayToMatrix(const std::vector<float>& values, int startIndex)
{
    int i = startIndex;
    Matrix4 transform(
                        values[i++], values[i++], values[i++], values[i++],
                        values[i++], values[i++], values[i++], values[i++],
                        values[i++], values[i++], values[i++], values[i++],
                        values[i++], values[i++], values[i++], values[i++]
                        );
    
    return transform;
}


std::string TColladaParserUtils::SkipStartChar(const std::string& s)
{
    size_t length = s.length();
    return s.substr(1, length-1);
}


TColladaSource* TColladaParserUtils::FindSourceById(const std::vector<TColladaSource*>& sources, const std::string& id)
{
    for(size_t j=0;j<sources.size();++j)
    {
        if( sources[j]->id == id )
            return sources[j];
    
    }
    return NULL;
}


