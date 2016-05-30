#ifndef __ColladaNewParam__
#define __ColladaNewParam__

#include "ColladaBase.h"


class TColladaSurface;
class TColladaSampler2D;

class TColladaNewParam : public TColladaBase
{
public:
    TColladaNewParam();
    ~TColladaNewParam();
    
    virtual TColladaBase* Parse(TiXmlElement* xml);
    
    std::string sid;
    
    TColladaSurface* surface;
    TColladaSampler2D* sampler2D;
};

#endif