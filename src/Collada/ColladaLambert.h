#ifndef __ColladaLambert__
#define __ColladaLambert__

#include "ColladaBase.h"

#include <vector>

class TColladaLambert : public TColladaBase
{
public:
    TColladaLambert();
    ~TColladaLambert();
    virtual TColladaBase* Parse(TiXmlElement* xml);
        
       
    std::string diffuseTexture;
    
};


#endif