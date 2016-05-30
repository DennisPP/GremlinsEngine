#ifndef __ColladaInstanceVisualScene__
#define __ColladaInstanceVisualScene__

#include "ColladaBase.h"



class TColladaInstanceVisualScene : public TColladaBase
{
public:
    
    TColladaBase* Parse(TiXmlElement* xml);
    std::string url;
    std::string name;
    std::string sid;
};


#endif