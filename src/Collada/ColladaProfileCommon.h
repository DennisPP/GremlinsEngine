#ifndef __ColladaProfileCommon__
#define __ColladaProfileCommon__

#include "ColladaBase.h"


class TColladaNewParam;
class TColladaTechnique;

class TColladaProfileCommon : public TColladaBase
{
public:
    TColladaProfileCommon();
    ~TColladaProfileCommon();
    virtual TColladaBase* Parse(TiXmlElement* xml);
    
    
    std::vector<TColladaNewParam*> newParams;
    TColladaTechnique* technique;
};



#endif