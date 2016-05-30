#ifndef __Quaternion_H__
#define __Quaternion_H__

#include "Math3D.h"

#include "Matrix3.h"

#include <cassert>


class Quaternion
{
public:
    inline Quaternion (float fW = 1.0, float fX = 0.0, float fY = 0.0, float fZ = 0.0);
    inline Quaternion (const Quaternion& rkQ);
    inline Quaternion(const float& rfAngle, const Vector3& rkAxis);
    inline Quaternion(float* valptr);
    inline Quaternion(const Matrix3& rot);
    inline float operator [] ( const size_t i ) const;
    inline float& operator [] ( const size_t i );
    inline float* ptr();
    inline const float* ptr() const;
    
    void FromAngleAxis (const float& rfAngle, const Vector3& rkAxis);
    void ToAngleAxis (float& rfAngle, Vector3& rkAxis) const;
    void ToAxes (Vector3* akAxis) const;
    void ToAxes (Vector3& xAxis, Vector3& yAxis, Vector3& zAxis) const;

    Vector3 xAxis(void) const;
    Vector3 yAxis(void) const;
    Vector3 zAxis(void) const;

    inline Quaternion& operator= (const Quaternion& rkQ);
    Quaternion operator+ (const Quaternion& rkQ) const;
    Quaternion operator- (const Quaternion& rkQ) const;
    Quaternion operator* (const Quaternion& rkQ) const;
    Quaternion operator* (float fScalar) const;
     friend Quaternion operator* (float fScalar, const Quaternion& rkQ);
    Quaternion operator- () const;
    inline bool operator== (const Quaternion& rhs) const;
    inline bool operator!= (const Quaternion& rhs) const;
  
    float Dot (const Quaternion& rkQ) const;  
    float Norm () const; 
   
    float normalise(void); 
    Quaternion Inverse () const; 
    Quaternion UnitInverse () const; 
   
    Vector3 operator* (const Vector3& rkVector) const;

    
    float getRoll(bool reprojectAxis = true) const;
    
    float getPitch(bool reprojectAxis = true) const;
    
    float getYaw(bool reprojectAxis = true) const;	

    void FromRotationMatrix (const Matrix3& kRot);
    void ToRotationMatrix ( Matrix3 & kRot) const;

    static Quaternion Slerp (float fT, const Quaternion& rkP,
        const Quaternion& rkQ, bool shortestPath = false);
   
    static Quaternion Squad (float fT, const Quaternion& rkP,
        const Quaternion& rkA, const Quaternion& rkB,
        const Quaternion& rkQ, bool shortestPath = false);
   
    static Quaternion nlerp(float fT, const Quaternion& rkP, 
        const Quaternion& rkQ, bool shortestPath = false);

    
    static const float ms_fEpsilon;
    static const Quaternion ZERO;
    static const Quaternion IDENTITY;
    float x, y, z, w;
};

inline Quaternion::Quaternion (float fW, float fX, float fY, float fZ)
{
    w = fW;
    x = fX;
    y = fY;
    z = fZ;
}

inline Quaternion::Quaternion (const Quaternion& rkQ)
{
    w = rkQ.w;
    x = rkQ.x;
    y = rkQ.y;
    z = rkQ.z;
}

inline Quaternion& Quaternion::operator= (const Quaternion& rkQ)
{
    w = rkQ.w;
    x = rkQ.x;
    y = rkQ.y;
    z = rkQ.z;
    return *this;
}

inline Quaternion::Quaternion(const float& rfAngle, const Vector3& rkAxis)
{
    this->FromAngleAxis(rfAngle, rkAxis);
}
inline Quaternion::Quaternion(float* valptr)
{
    memcpy(&w, valptr, sizeof(float)*4);
}

inline Quaternion::Quaternion(const Matrix3& rot)
{
    this->FromRotationMatrix(rot);
}


inline float Quaternion::operator [] ( const size_t i ) const
{
    assert( i < 4 );
    
    return *(&w+i);
}

inline float& Quaternion::operator [] ( const size_t i )
{
    assert( i < 4 );
    
    return *(&w+i);
}

inline float* Quaternion::ptr()
{
    return &w;
}

inline const float* Quaternion::ptr() const
{
    return &w;
}

inline bool Quaternion::operator== (const Quaternion& rhs) const
{
    return (rhs.x == x) && (rhs.y == y) &&
    (rhs.z == z) && (rhs.w == w);
}

inline bool Quaternion::operator!= (const Quaternion& rhs) const
{
    return !operator==(rhs);
}

#endif
