#ifndef __ColladaNode__
#define __ColladaNode__

#include "ColladaBase.h"

#include "Math3D/Matrix4.h"

class TColladaInstanceController;
class TColladaInstanceGeometry;

class TColladaNode : public TColladaBase
{
public:
    TColladaNode();
    TColladaNode(const Matrix4& transform, int pid = -1);
    ~TColladaNode();
    
   
    
    void AddChild(TColladaNode* child);
    
    void UpdateTransform(const Matrix4& t = Matrix4::IDENTITY);
    void Dump(int depth=0);
    
    bool IsRootNode() const;
    bool IsJointNode() const;
    
    bool IsValid() const;
    
    int TreeSize() const;
    
    TColladaNode* parentNode;
    
    
    std::string name;
    std::string sid;
    std::vector<TColladaNode*> childs;
    
    std::vector<TColladaInstanceController*> instanceControllers; // 0 or more
    std::vector<TColladaInstanceGeometry*> instanceGeometrys;   // 0 or more
    
    
    std::string type;
    Matrix4 baseTransform;
    Matrix4 transform;
    Matrix4 worldTransform;
    Quaternion orientation;
    Vector3 position;
    int nodeId;
    int parentId;
};

#endif /* defined(__ModelViewer__ColladaNode__) */
