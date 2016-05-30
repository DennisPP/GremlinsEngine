#ifndef __ColladaController__
#define __ColladaController__

#include "ColladaBase.h"

class TColladaSkin;
class TColladaMorph;

class TColladaController : public TColladaBase
{
public:
    TColladaController();
    ~TColladaController();
    virtual TColladaBase* Parse(TiXmlElement* xml);
    TColladaSkin* skin;
    TColladaMorph* morph;
};


#endif