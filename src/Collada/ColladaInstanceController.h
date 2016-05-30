#ifndef __ColladaInstanceController__
#define __ColladaInstanceController__

#include "ColladaBase.h"


class TColladaBindMaterial;

class TColladaInstanceController : public TColladaBase
{
public:
    TColladaInstanceController();
    ~TColladaInstanceController();
    virtual TColladaBase* Parse(TiXmlElement* xml);
  
    std::string url; // pointer to controller
    std::vector<std::string> skeletons;
    
    
    TColladaBindMaterial* bindMaterial; 
    
};

#endif