#ifndef __MathUtil__h__
#define __MathUtil__h__

#include "Math3D.h"

#include "Ray.h"

#include <vector>

class TMathUtil
{
public:
	static void GetBary(
					  Vector3 & pt
                    , Vector3 & origin
					, Vector3 & U
					, Vector3 & V
					, float * u
					, float * v
					);	

	static float GetRayToPlaneDistance(
                                const Ray & ray
                            ,   const Vector3 & pos
                            ,   const Vector3 & axis1
                            ,   const Vector3 & axis2
                            ,	float size
							,	bool bothSide = false
                            );

	static float GetRayToInfinityPlaneDistance(
					          const Ray & ray
					        , const Vector3 & pos
					        , const Vector3 & axis1
					        , const Vector3 & axis2
							,	float * u
							,	float * v
							, bool bothSide = false
					        );

	static float SmoothStep(float a, float b, float x);
	static bool IsInside(const std::vector<Vector2> & polygon,  const Vector2 & pt);

	static int GetNextPower2(int width);

	static Vector3 GetProjectedPoint(const Vector3 & from, const Vector3 & v, const Vector3 & pt);
};

void AssertIsNumber(float x);

#endif