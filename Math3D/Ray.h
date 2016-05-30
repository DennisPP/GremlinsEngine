
#ifndef __FireEngine__Math3D__Ray_H_
#define __FireEngine__Math3D__Ray_H_

#include "Vector3.h"

//------------------------------------------------------------------------------
class Ray
{
protected:
    Vector3 mOrigin;
    Vector3 mDirection;
public:
    Ray();
    Ray(const Vector3& origin, const Vector3& direction);  
    void setOrigin(const Vector3& origin);
  
    const Vector3& getOrigin(void) const;   
    void setDirection(const Vector3& dir);
   
    const Vector3& getDirection(void) const;
	
	Vector3 getPoint(float t) const ;

	/** Gets the position of a point t units along the ray. */
	Vector3 operator*(float t) const;

	std::pair<bool, float> intersects(const Plane& p) const;
	std::pair<bool, float> intersects(const Sphere& s) const;
};
//------------------------------------------------------------------------------

#endif
