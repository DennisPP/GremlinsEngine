#ifndef __ColladaAsset__
#define __ColladaAsset__

#include "ColladaBase.h"

class TiXmlElement;

typedef enum TUpAxisType
{
    X_UP = 0,
    Y_UP,
    Z_UP
} TUpAxisType;

class TColladaAsset
{
public:
    TColladaAsset();
    void Parse(TiXmlElement* xml);
    TUpAxisType upAxis;
};


#endif