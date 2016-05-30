#include "Math3D.h"

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Ray.h"
#include "Sphere.h"
#include "Plane.h"

//------------------------------------------------------------------------------
const float Math::PI		= float( 4.0f * atan( 1.0f ) );
const float Math::TWO_PI	= float( 2.0f * PI );
const float Math::HALF_PI	= float( 0.5f * PI );
	
//------------------------------------------------------------------------------
Math::Math()
{
}

//------------------------------------------------------------------------------
Math::~Math()
{
}

//------------------------------------------------------------------------------
std::pair<bool, float> Math::intersects(const Ray& ray, const Plane& plane)
{

    float denom = plane.normal.dotProduct(ray.getDirection());
    if (Math::Abs(denom) < std::numeric_limits<float>::epsilon())
    {
        // Parallel
        return std::pair<bool, float>(false, 0.0f);
    }
    else
    {
        float nom = plane.normal.dotProduct(ray.getOrigin()) + plane.d;
        float t = -(nom/denom);
        return std::pair<bool, float>(t >= 0, t);
    }        
}   
//------------------------------------------------------------------------------
std::pair<bool, float> Math::intersects(
												const Ray& ray
											,	const Sphere& sphere
											,	bool discardInside
											)
{
    const Vector3& raydir = ray.getDirection();
    const Vector3& rayorig = ray.getOrigin() - sphere.getCenter();
    float radius = sphere.getRadius();

    if (rayorig.squaredLength() <= radius*radius && discardInside)
    {
        return std::pair<bool, float>(true, 0.0f);
    }

    float a = raydir.dotProduct(raydir);
    float b = 2 * rayorig.dotProduct(raydir);
    float c = rayorig.dotProduct(rayorig) - radius*radius;

    float d = (b*b) - (4 * a * c);
    if (d < 0.0f)
    {
        return std::pair<bool, float>(false, 0.0f);
    }
    else
    {
        float t = ( -b - Math::Sqrt(d) ) / (2 * a);
        if (t < 0.0f)
		{
            t = ( -b + Math::Sqrt(d) ) / (2 * a);
		}
        return std::pair<bool, float>(true, t);
    }
}   

//------------------------------------------------------------------------------
void Math::GetBary(
				  const Vector3 & pt
                , const Vector3 & origin
				, const Vector3 & U
				, const Vector3 & V
				, float * u
				, float * v
				)
{		

	Vector3 v0 = U;
	Vector3 v1 = V;
	Vector3 v2 = pt - origin;

	float dot00 = v0.dotProduct( v0 );
	float dot01 = v0.dotProduct( v1 );
	float dot02 = v0.dotProduct( v2 );
	float dot11 = v1.dotProduct( v1 );
	float dot12 = v1.dotProduct( v2 );

	float deno = ( dot00 * dot11 - dot01 * dot01);
	assert( deno != 0.0f );

	float inv = 1.0f / deno;
	*u = ( dot11 * dot02 - dot01 * dot12 ) * inv;
	*v = ( dot00 * dot12 - dot01 * dot02 ) * inv;	
}

//------------------------------------------------------------------------------
float Math::SmoothStep(float a, float b, float x)
{
    // Scale, bias and saturate x to 0..1 range
    if( x < a)
		return 0.0f;
	if( x >= b )
		return 1.0f;
    x = (x - a) / (b - a); 
	return x * x * (3.0f - 2.0f * x);
}

//------------------------------------------------------------------------------
float Math::Clamp(float min, float max, float x)
{
	x = x <= min ? min : x;
	x = x >= max ? max : x;
	return x;
}

//------------------------------------------------------------------------------
float Math::Lerp(float a, float b, float t)
{
	return a + ( b - a ) * t;
}

//------------------------------------------------------------------------------