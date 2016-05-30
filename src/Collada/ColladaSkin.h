#ifndef __ColladaSkin__
#define __ColladaSkin__


#include "ColladaBase.h"

#include "Math3D/Matrix4.h"

class TColladaInput;
class TColladaSource;
class TColladaNode;


class TColladaJoints : public TColladaBase
{
public:
    virtual TColladaBase* Parse(TiXmlElement* xml);
    
    std::vector<TColladaInput*> inputs;
};

class TColladaVertexWeights : public TColladaBase
{
public:
    void Prepare();
    int count;
    
    
    
    int GetJointIndex(int vertexIndex) const;
    
    
    std::vector<TColladaInput*> inputs;
    std::vector<int> vcount;
    std::vector<int> v;
    
    std::vector<int> startJointIndices; // start joint index for vertex
    
};


class TColladaSkin : public TColladaBase
{
public:
    TColladaSkin();
    ~TColladaSkin();
    virtual TColladaBase* Parse(TiXmlElement* xml);
    
    
    TColladaSource* GetWeightSource() const;
    TColladaSource* GetJointNameSource() const;
    TColladaSource* GetBindPoseSource() const;
    
    int GetJointIndex(TColladaNode* jointNode) const;
    
    std::string source; // pointer to geometry 
    std::vector<TColladaSource*> sources;
    
    Matrix4 bindShapeMatrix;
    
    TColladaJoints* joints;
    TColladaVertexWeights* vertexWeights;
    std::vector<Matrix4> invBindPoses;
};



#endif