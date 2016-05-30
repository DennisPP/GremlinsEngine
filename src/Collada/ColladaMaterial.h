#ifndef __ColladaMaterial__
#define __ColladaMaterial__

#include "ColladaBase.h"
#include "ColladaAsset.h"

#include <string>
#include <vector>

class TColladaInstanceEffect;

class TColladaMaterial : public TColladaBase
{
public:
    TColladaMaterial();
    ~TColladaMaterial();
    
     virtual TColladaBase* Parse(TiXmlElement* xml);
    
    TColladaAsset* asset;
    
    
    std::vector<TColladaInstanceEffect*> instanceEffects;
    std::string name;
};

#endif