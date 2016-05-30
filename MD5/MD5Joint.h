#ifndef _MD5_JOINT_H_
#define _MD5_JOINT_H_


#include "Math3D/Vector3.h"
#include "Math3D/Quaternion.h"
#include "Math3D/Matrix4.h"

class TMD5Joint
{
	friend class TMD5Model;
    friend class TMD5Renderer;
public:
	TMD5Joint();
	~TMD5Joint();
	
	void SetParent(int id);
	void SetName(const char* name);
	void SetPosition(const Vector3& pos);
	void SetOrientation(const Quaternion& orientation);
	void AddChild(TMD5Joint* joint);
	const Vector3& GetPosition() const;
	int ChildCount() const;
	TMD5Joint* GetChild(int idx) const;

    inline int GetParentId() const;

	Quaternion mOrientation;
    Matrix4 transform;
    Matrix4 bindPose;
    Matrix4 inverseBindPose;
    // for debug purpose only
    Vector3 mPositionLoaded;
    
    
protected:
	char* mName;
	int mParentId;
	Vector3 mPosition;
	
    

	TMD5Joint* mParent;
	std::vector<TMD5Joint*> mChilds;
};

int TMD5Joint::GetParentId() const
{
    return mParentId;
}


class TMD5Skeleton
{
public:
    TMD5Skeleton();
    
    
    std::vector<TMD5Joint*> mJoints;
};

#endif