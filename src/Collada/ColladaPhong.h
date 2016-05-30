#ifndef __ColladaPhong__
#define __ColladaPhong__


#include "ColladaBase.h"

#include <vector>

class TColladaPhong : public TColladaBase
{
public:
    TColladaPhong();
    ~TColladaPhong();
    virtual TColladaBase* Parse(TiXmlElement* xml);
    
    std::string diffuseTexture;
    
};

#endif