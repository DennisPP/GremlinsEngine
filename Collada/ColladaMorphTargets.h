#ifndef __ColladaMorphTarget__
#define __ColladaMorphTarget__

#include "ColladaBase.h"

class TColladaInput;

class TColladaMorphTargets : public TColladaBase
{
public:
    virtual TColladaBase* Parse(TiXmlElement* xml);
    std::vector<TColladaInput*> inputs;
};

#endif