#ifndef __ColladaScene__
#define __ColladaScene__

#include "ColladaBase.h"


class TColladaInstanceVisualScene;


class TColladaScene : public TColladaBase
{
public:
    virtual TColladaBase* Parse(TiXmlElement* xml);
    std::vector<TColladaInstanceVisualScene*> instanceVisualScenes;

};


#endif