#ifndef __ColladaEffect__
#define __ColladaEffect__

#include "ColladaBase.h"

#include <string>


class TColladaProfileCommon;

class TColladaEffect : public TColladaBase
{
public:
    TColladaEffect();
    ~TColladaEffect();
    virtual TColladaBase* Parse(TiXmlElement* xml);
    
    
    
    
    std::string name;
    
    TColladaProfileCommon* profileCommon;
};



#endif