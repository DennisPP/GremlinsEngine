#ifndef __ColladaCamera__
#define __ColladaCamera__

#include "ColladaBase.h"

class TColladaCamera : public TColladaBase
{
public:
    virtual TColladaBase* Parse(TiXmlElement* xml);

};


#endif