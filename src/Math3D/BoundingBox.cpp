#include "BoundingBox.h"

TBoundingBox::TBoundingBox()
{
	float inf = std::numeric_limits<float>::infinity();
	mMinPt = Vector3(inf, inf, inf);
	mMaxPt = Vector3(-inf, -inf, -inf);

}

TBoundingBox::TBoundingBox(const Vector3& minPt, const Vector3& maxPt)
{
	mMinPt = minPt;
	mMaxPt = maxPt;
	assert(IsValid());
}

bool TBoundingBox::IsValid() const
{
	float x = mMaxPt.x - mMinPt.x;
	float y = mMaxPt.y - mMinPt.y;
	float z = mMaxPt.z - mMinPt.z;

	return x>=0.0f && y>=0.0f && z>=0.0f;
}

Vector3 TBoundingBox::GetCenterPoint() const
{
	return 0.5f * (mMinPt + mMaxPt);
}

Vector3 TBoundingBox::GetDimention() const
{	
	float x = mMaxPt.x - mMinPt.x;
	float y = mMaxPt.y - mMinPt.y;
	float z = mMaxPt.z - mMinPt.z;


	assert( IsValid() );
	
	return Vector3(x, y, z);
}