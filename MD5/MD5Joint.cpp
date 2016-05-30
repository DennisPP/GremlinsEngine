#include "MD5Joint.h"



TMD5Joint::TMD5Joint()
{
	mParent = NULL;
	mName = NULL;
}

TMD5Joint::~TMD5Joint()
{

	if( mName )
	{
		delete [] mName;
	}
}	


void TMD5Joint::SetParent(int id)
{
	mParentId = id;
}

int TMD5Joint::ChildCount() const
{
	return (int)mChilds.size();
}

TMD5Joint* TMD5Joint::GetChild(int idx) const
{
	assert( 0<=idx && idx<ChildCount() );
	return mChilds[idx];
}

void TMD5Joint::SetName(const char* name)
{
	if( mName )
	{
		delete [] mName;
		mName = NULL;
	}
	mName = strdup(name);
}

void TMD5Joint::SetPosition(const Vector3& pos)
{
	mPosition = pos;
}

const Vector3& TMD5Joint::GetPosition() const
{
	return mPosition;
}

void TMD5Joint::SetOrientation(const Quaternion& orientation)
{
	mOrientation = orientation;
}
	
void TMD5Joint::AddChild(TMD5Joint* joint)
{
	joint->mParent = this;
	mChilds.push_back(joint);
}