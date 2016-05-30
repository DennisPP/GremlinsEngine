#ifndef __ColladaSource__
#define __ColladaSource__

#include "ColladaBase.h"

#include <vector>
#include <string>

class TColladaNameArray : public TColladaBase
{
public:
    std::vector<std::string> names;
    TColladaBase* Parse(TiXmlElement* xml);
};

class TColladaIDREFArray : public TColladaBase
{
public:
    TColladaIDREFArray();
    TColladaBase* Parse(TiXmlElement* xml);
    
    int count;
    std::string name;
    
    std::vector<std::string> ids;
    
};

class TColladaParam : public TColladaBase
{
public:
    
    TColladaBase* Parse(TiXmlElement* xml);
    
    std::string name;
    std::string type;
};


class TColladaAccessor : public TColladaBase
{
public:
    
    TColladaBase* Parse(TiXmlElement* xml);
    
    int offset;
    std::string source;
    int count;
    int stride;
    
    int GetParamOffset(TColladaParam* p) const;
    
    
    std::vector<TColladaParam*> params;
};

class TColladaSource : public TColladaBase
{
public:
    TColladaSource();
    
    virtual TColladaBase* Parse(TiXmlElement* xml);
    
    TColladaParam* GetParamByName(const std::string& name) const;

    
    int stride; // the stride in the accessor
    TColladaAccessor* accessor;
    
    TColladaNameArray* nameArray;
    TColladaIDREFArray* idRefArray;
    std::vector<float> values;
    std::string name;
};

#endif /* defined(__ModelViewer__ColladaSource__) */
