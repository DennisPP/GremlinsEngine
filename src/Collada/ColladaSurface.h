#ifndef __ColladaSurface__
#define __ColladaSurface__

#include "ColladaBase.h"

class TColladaSurface : TColladaBase
{
public:
    TColladaSurface();
    ~TColladaSurface();
    
    virtual TColladaBase* Parse(TiXmlElement* xml);
    std::string type;
    std::string initFrom;
};

#endif