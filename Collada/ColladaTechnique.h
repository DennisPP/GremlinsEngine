#ifndef __ColladaTechnique__
#define __ColladaTechnique__

#include "ColladaBase.h"

class TColladaLambert;
class TColladaPhong;

class TColladaTechnique : public TColladaBase
{
public:
    TColladaTechnique();
    ~TColladaTechnique();
    
    virtual TColladaBase* Parse(TiXmlElement* xml);
    
    TColladaLambert* lambert;
    TColladaPhong* phong;

};

#endif