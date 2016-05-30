#ifndef _MD5ANIMATION_H_
#define _MD5ANIMATION_H_

#include "Math3D/Vector3.h"

#include "MD5Joint.h"


#include <string>


class TFrameData
{
public:
	std::vector<float> mData;
};

class TBaseFrame
{
public:
	Vector3 mPosition;
	Vector3 mOrientation;
};

class TMD5JointInfo
{
public:
	TMD5JointInfo();
	TMD5JointInfo(const char* name, int parent, int flags, int startIndex);
	~TMD5JointInfo();

	
protected:
public:
	std::string mName;
	int mParentId;
	int mFlags;
	int mStartIndex;
};

class TFrameSkeleton
{
	public:

	std::vector<TMD5Joint> mJoints;	
};

class TMD5Animation
{
public:
	TMD5Animation();
	~TMD5Animation();

	bool Load(const char* path);
    
   
protected:
public:
	void ParseHierarchy(FILE* fp);
	void ParseBounds(FILE* fp);
	void ParseBaseframe(FILE* fp);
	void ParseFrame(FILE* fp);
    static Quaternion BuildQuaternion(float qx, float qy, float qz);


	int mNumAnimatedComponents;
	int mNumJoints;
	float mFrameRate;

	std::vector<TMD5JointInfo> mJointInfos;
	std::vector<TBaseFrame> mBaseFrames;
	std::vector<TFrameData> mFrameData;

	

};




#endif