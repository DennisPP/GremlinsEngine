#ifndef __Sphere_H_
#define __Sphere_H_

#include "Vector3.h"



class Sphere
{
protected:
    float mRadius;
    Vector3 mCenter;
public:
   
    Sphere() : mRadius(1.0), mCenter(Vector3::ZERO) {}
    Sphere(const Vector3& center, float radius)
        : mRadius(radius), mCenter(center) {}

   
    float getRadius(void) const { return mRadius; }

   
    void setRadius(float radius) { mRadius = radius; }

    const Vector3& getCenter(void) const { return mCenter; }

    void setCenter(const Vector3& center) { mCenter = center; }

	bool intersects(const Sphere& s) const
	{
        return (s.mCenter - mCenter).squaredLength() <=
            Math::Sqr(s.mRadius + mRadius);
	}
	/*bool intersects(const Plane& plane) const
	{
		return Math::intersects(*this, plane);
	}*/
	bool intersects(const Vector3& v) const
	{
        return ((v - mCenter).squaredLength() <= Math::Sqr(mRadius));
	}
};


#endif

