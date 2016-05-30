#include "Ray.h"

//------------------------------------------------------------------------------
Ray::Ray()
:	mOrigin(Vector3::ZERO)
,	mDirection(Vector3::UNIT_Z) 
{
}
 
//------------------------------------------------------------------------------
Ray::Ray(const Vector3& origin, const Vector3& direction)
:	mOrigin(origin), mDirection(direction) 
{
}

//------------------------------------------------------------------------------
void Ray::setOrigin(const Vector3& origin) 
{
	mOrigin = origin;
} 

//------------------------------------------------------------------------------  
const Vector3 & Ray::getOrigin(void) const 
{
	return mOrigin;
} 
  
//------------------------------------------------------------------------------
void Ray::setDirection(const Vector3& dir) 
{
	mDirection = dir;
} 
   
//------------------------------------------------------------------------------
const Vector3 & Ray::getDirection(void) const
{
	return mDirection;
} 

//------------------------------------------------------------------------------
Vector3 Ray::getPoint(float t) const 
{ 
	return Vector3(mOrigin + (mDirection * t));
}
	
//------------------------------------------------------------------------------
Vector3 Ray::operator*(float t) const 
{ 
	return getPoint(t);
}

//------------------------------------------------------------------------------
std::pair<bool, float> Ray::intersects(const Plane& p) const
{
	return Math::intersects(*this, p);
}      

//------------------------------------------------------------------------------
std::pair<bool, float> Ray::intersects(const Sphere& s) const
{
	return Math::intersects(*this, s);
}
//------------------------------------------------------------------------------