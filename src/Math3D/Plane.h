
#ifndef __Plane_H__
#define __Plane_H__



#include "Vector3.h"
#include "Vector4.h"



  
class  Plane
{
public:
       
    Plane ();
    Plane (const Plane& rhs);
        Plane (const Vector3& rkNormal, float fConstant);
    Plane (const Vector3& rkNormal, const Vector3& rkPoint);
    Plane (const Vector3& rkPoint0, const Vector3& rkPoint1,
        const Vector3& rkPoint2);

     
    enum Side
    {
        NO_SIDE,
        POSITIVE_SIDE,
        NEGATIVE_SIDE,
        BOTH_SIDE
    };

    Side getSide (const Vector3& rkPoint) const;

      
    Side getSide (const Vector3& centre, const Vector3& halfSize) const;

      
    float getSignedDistance (const Vector3& rkPoint) const;

      
    void redefine(const Vector3& rkPoint0, const Vector3& rkPoint1,
        const Vector3& rkPoint2);

		
	void redefine(const Vector3& rkNormal, const Vector3& rkPoint);

	Vector3 GetMirrorPoint(const Vector3& pt) const;
	Vector3 GetMirrorVector(const Vector3& v) const;
	
    float normalise(void);

	Vector3 normal;
    float d;

    /// Comparison operator
    bool operator==(const Plane& rhs) const
    {
        return (rhs.d == d && rhs.normal == normal);
    }
    bool operator!=(const Plane& rhs) const
    {
        return (rhs.d != d && rhs.normal != normal);
    }      
};  


#endif