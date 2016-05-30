#ifndef __ColladaInstanceGeometry__
#define __ColladaInstanceGeometry__

#include "ColladaBase.h"



class TColladaBindMaterial;
class TColladaNode;

class TColladaInstanceGeometry : public TColladaBase
{
public:
    TColladaInstanceGeometry(TColladaNode* parentNode);
    virtual TColladaBase* Parse(TiXmlElement* xml);
    
    inline TColladaNode* GetParentNode() const;
    
    
    
    std::string name;
    std::string sid;
    std::string url;
  
    
    TColladaNode* parentNode;
    
    TColladaBindMaterial* bindMaterial;
};

TColladaNode* TColladaInstanceGeometry::GetParentNode() const
{
    return parentNode;
}


#endif