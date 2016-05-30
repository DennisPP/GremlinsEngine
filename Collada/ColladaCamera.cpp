#include "ColladaCamera.h"


TColladaBase* TColladaCamera::Parse(TiXmlElement* xml)
{
    TColladaBase::Parse(xml);
    return this;
}