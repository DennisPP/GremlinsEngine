#ifndef __ColladaImage__
#define __ColladaImage__

#include "ColladaBase.h"

#include <string>


class TColladaImage : public TColladaBase
{
public:
    TColladaImage();
    
    virtual TColladaBase* Parse(TiXmlElement* xml);
    
    std::string name;
    
    std::string path;
};



#endif