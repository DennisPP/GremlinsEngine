#ifndef __ColladaBindMaterial__
#define __ColladaBindMaterial__

#include "ColladaBase.h"


class TColladaInstanceMaterial;

class TColladaBindMaterial : public TColladaBase
{
public:
    TColladaBindMaterial();
    ~TColladaBindMaterial();
    
    virtual TColladaBase* Parse(TiXmlElement* xml);
    
    
    TColladaInstanceMaterial* instanceMaterial;
};


#endif