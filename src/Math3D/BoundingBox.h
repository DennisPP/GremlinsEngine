#ifndef _FIREENGINE_BOUNDING_BOX_H_
#define _FIREENGINE_BOUNDING_BOX_H_

#include "Vector3.h"

class TBoundingBox
{
public:
	TBoundingBox();
	TBoundingBox(const Vector3& minPt, const Vector3& maxPt);
	Vector3 GetCenterPoint() const;
	Vector3 GetDimention() const;

	bool IsValid() const;

	Vector3 mMinPt, mMaxPt;
	
};

#endif