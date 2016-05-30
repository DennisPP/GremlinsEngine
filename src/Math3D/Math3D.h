#ifndef __Math_H__
#define __Math_H__

#include <vector>
#include <cassert>
#include <limits>

#include <math.h>

class Vector2;
class Vector3;
class Vector4;
class Ray;
class Sphere;
class Plane;
class Matrix3;
class Math 
{  
  
public:

	static const float PI;
    static const float TWO_PI;
    static const float HALF_PI;
   
    Math();      
    ~Math();
	static  float ToRadian( float degree )
	{
		return PI * degree / 180.0f;
	}
	static float ToDegree(float radian)
	{
		return radian * 180.0f / PI;
	}
	static float RangeRandom( float a, float b )
	{
		return a + ( b - a ) * ( (float)( rand() % RAND_MAX) / RAND_MAX );
	}

	static inline float Abs (float fValue) { return float(fabs(fValue)); }
	static inline float Sqrt (float fValue) { return float(sqrt(fValue)); }

	static inline float Sqr (float fValue) { return fValue*fValue; }

	static std::pair<bool, float> intersects(const Ray& ray, const Plane& plane);   
	static std::pair<bool, float> intersects(const Ray& ray, const Sphere& sphere, 
												bool discardInside = true);

	static inline float ATan (float fValue) { return (atan(fValue)); }
	static inline float ATan2 (float fY, float fX) { return (atan2(fY,fX)); }
	
	static inline float Sin (float fValue )
	{
		return sin(fValue);
	}

	static inline float Cos (const float& fValue )
	{
			return cos( fValue);
	}
	
	static inline float ACos (float fValue)
	{
		if ( -1.0 < fValue )
		{
			if ( fValue < 1.0 )
				return float(acos(fValue));
			else
				return float(0.0);
		}
		else
		{
			return float(PI);
		}
	}
	
	static inline float ASin (float fValue)
	{
		if ( -1.0 < fValue )
		{
			if ( fValue < 1.0 )
				return float(asin(fValue));
			else
				return float(HALF_PI);
		}
		else
		{
			return float(-HALF_PI);
		}
	}
	
	static inline float InvSqrt(float fValue)
	{
		assert( fValue != 0.0f );
		return float( 1.0f / sqrt(fValue));
	}       

	static void GetBary(
							const Vector3 & pt
						,	const Vector3 & origin
						,	const Vector3 & U
						,	const Vector3 & V
						,	float * u
						,	float * v
						);
	static float SmoothStep(float a, float b, float x);	

	static float Clamp(float min, float max, float x);

	static float Lerp(float a, float b, float t);
};	


#endif
