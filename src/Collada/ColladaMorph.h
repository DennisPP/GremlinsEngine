#ifndef __ColladaMorph__
#define __ColladaMorph__

#include "ColladaBase.h"


#include <vector>

class TColladaSource;
class TColladaMorphTargets;
class TColladaMorph : public TColladaBase
{
public:
    TColladaMorph();
    TColladaBase* Parse(TiXmlElement* xml);
    
    
    TColladaSource* GetMorphTargetSource() const;
    TColladaSource* GetMorphWeightSource() const;
    
    
    std::string method;
    std::string source;
    
    std::vector<TColladaSource*> sources;   // 2 or more
    std::vector<TColladaMorphTargets*> morphTargets;
protected:
    TColladaSource* GetSourceById(const std::string& id) const;
};


#endif