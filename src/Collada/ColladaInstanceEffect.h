#ifndef __ColladaInstanceEffect__
#define __ColladaInstanceEffect__

#include "ColladaBase.h"

#include <string>

class TColladaInstanceEffect : public TColladaBase
{
public:
    TColladaInstanceEffect();
    ~TColladaInstanceEffect();
    
    virtual TColladaBase* Parse(TiXmlElement* xml);
    
    std::string url;
    
};



#endif