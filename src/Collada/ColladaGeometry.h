#ifndef __ColladaGeometry__
#define __ColladaGeometry__

#include "ColladaBase.h"

class TColladaMesh;

class TColladaGeometry : public TColladaBase
{
public:
    TColladaGeometry();
    virtual TColladaBase* Parse(TiXmlElement* xml);
    
    
    TColladaMesh* mesh;
    std::string name;
};

#endif