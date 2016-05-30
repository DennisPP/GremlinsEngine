#ifndef __ColladaSampler2D__
#define __ColladaSampler2D__

#include "ColladaBase.h"

#include <string>


class TColladaSampler2D : public TColladaBase
{
public:
    TColladaSampler2D();
    ~TColladaSampler2D();
    
    virtual TColladaBase* Parse(TiXmlElement* xml);
    
    std::string source;

};

#endif