#ifndef __GBOJoint__
#define __GBOJoint__

#include "Math3D/Vector3.h"
#include "Math3D/Matrix4.h"
#include "Math3D/Quaternion.h"

#include <string>

class TGBOJoint
{
public:
    TGBOJoint();
    ~TGBOJoint();
    int parentId;
    Vector3 position;
    Quaternion orientation;
    std::string name;
    Matrix4 transform;
    Matrix4 worldTransform;
    
    void UpdateTransform(const Matrix4& mat = Matrix4::IDENTITY);
    
    
    void AddChild(TGBOJoint* child);
    void Dump(int depth=0);
    
    TGBOJoint* parent;
    
    Matrix4 inverseBindPose;
    
    std::vector<TGBOJoint*> childs;
};



#endif