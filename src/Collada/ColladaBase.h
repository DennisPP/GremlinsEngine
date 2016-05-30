#ifndef __ColladaBase__
#define __ColladaBase__

#include <string>

#include <vector>

#include "Math3D/Matrix4.h"


class TColladaSource;

class TiXmlElement;
class TColladaBase
{
public:
    TColladaBase();
    virtual ~TColladaBase();
    virtual TColladaBase* Parse(TiXmlElement* xml);
    std::string id;
};

class Vector3;
class Vector4;
class TColladaParserUtils
{
public:
    
    static void ParseVector3(const char* text, Vector3& v);
    static void ParseVector4(const char* text, Vector4& v);
    static void ParseFloatArray(const char* text, std::vector<float>& values);
    static void ParseIntArray(const char* text, std::vector<int>& values);
    static void ParseNameArray(const char* text, std::vector<std::string>& names);
    static Matrix4 FloatArrayToMatrix(const std::vector<float>& values, int startIndex);
    
    static std::string SkipStartChar(const std::string& s);
    static TColladaSource* FindSourceById(const std::vector<TColladaSource*>& sources, const std::string& id);

};


#endif