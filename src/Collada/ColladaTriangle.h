#ifndef __ColladaTriangle__
#define __ColladaTriangle__

#include "ColladaBase.h"

class TColladaInput;

class TColladaTriangle : public TColladaBase
{
public:
    
    virtual TColladaBase* Parse(TiXmlElement* xml);
    
    bool UseTextureIndex() const;
    
    
    std::vector<TColladaInput*> inputs;
    std::string material;
    
    
    
    int triangleCount;
    
    std::string vertexSource;
    std::string texCoordSource;
    std::string normalSource;
    std::vector<int> p;
};

#endif