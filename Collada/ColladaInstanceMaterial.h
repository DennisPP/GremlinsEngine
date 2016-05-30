#ifndef __ColladaInstanceMaterial__
#define __ColladaInstanceMaterial__

#include "ColladaBase.h"

#include <string>

class TColladaInstanceMaterial : public TColladaBase
{
public:
    virtual TColladaBase* Parse(TiXmlElement* xml);
    std::string symbol;
    std::string target;
};


#endif