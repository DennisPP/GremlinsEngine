#include "Plane.h"


//-----------------------------------------------------------------------------------
Plane::Plane ()
{
	normal = Vector3::ZERO;
	d = 0.0;
}
//-----------------------------------------------------------------------------------
Plane::Plane (const Plane& rhs)
{
	normal = rhs.normal;
	d = rhs.d;
}
//-----------------------------------------------------------------------------------
Plane::Plane (const Vector3& rkNormal, float fConstant)
{
	normal = rkNormal;
	d = -fConstant;
}
//-----------------------------------------------------------------------------------
Plane::Plane (const Vector3& rkNormal, const Vector3& rkPoint)
{
	redefine(rkNormal, rkPoint);
}
//-----------------------------------------------------------------------------------
Plane::Plane (const Vector3& rkPoint0, const Vector3& rkPoint1,
	const Vector3& rkPoint2)
{
	redefine(rkPoint0, rkPoint1, rkPoint2);
}
//-----------------------------------------------------------------------------------
float Plane::getSignedDistance (const Vector3& rkPoint) const
{
	return normal.dotProduct(rkPoint) + d;
}
//-----------------------------------------------------------------------------------
Plane::Side Plane::getSide (const Vector3& rkPoint) const
{
	float fDistance = getSignedDistance(rkPoint);

	if ( fDistance < 0.0 )
		return Plane::NEGATIVE_SIDE;

	if ( fDistance > 0.0 )
		return Plane::POSITIVE_SIDE;

	return Plane::NO_SIDE;
}


//-----------------------------------------------------------------------------------
Plane::Side Plane::getSide (const Vector3& centre, const Vector3& halfSize) const
{
    // Calculate the distance between box centre and the plane
    float dist = getSignedDistance(centre);

    // Calculate the maximise allows absolute distance for
    // the distance between box centre and plane
    float maxAbsDist = normal.absDotProduct(halfSize);

    if (dist < -maxAbsDist)
        return Plane::NEGATIVE_SIDE;

    if (dist > +maxAbsDist)
        return Plane::POSITIVE_SIDE;

    return Plane::BOTH_SIDE;
}
//-----------------------------------------------------------------------------------
void Plane::redefine(const Vector3& rkPoint0, const Vector3& rkPoint1,
	const Vector3& rkPoint2)
{
	Vector3 kEdge1 = rkPoint1 - rkPoint0;
	Vector3 kEdge2 = rkPoint2 - rkPoint0;
	normal = kEdge1.crossProduct(kEdge2);
	normal.normalise();
	d = -normal.dotProduct(rkPoint0);
}
//-----------------------------------------------------------------------------------
void Plane::redefine(const Vector3& rkNormal, const Vector3& rkPoint)
{
	normal = rkNormal;
	d = -rkNormal.dotProduct(rkPoint);
}

//-----------------------------------------------------------------------------------
float Plane::normalise(void)
{
    float fLength = normal.length();

    // Will also work for zero-sized vectors, but will change nothing
    if (fLength > 1e-08f)
    {
        float fInvLength = 1.0f / fLength;
        normal *= fInvLength;
        d *= fInvLength;
    }

    return fLength;
}

Vector3 Plane::GetMirrorPoint(const Vector3& pt) const
{
	float dist = normal.x * pt.x + normal.y * pt.y + normal.z * pt.z + d;
	return pt - 2.0f * dist * normal;
}

Vector3 Plane::GetMirrorVector(const Vector3& v) const
{
	float dist = normal.x * v.x + normal.y * v.y + normal.z * v.z;
	return v - 2.0f * dist * normal;
}