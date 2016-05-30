#include "MathUtil.h"

#include "Vector2.h"
#include "Vector3.h"


#include <cassert>

//------------------------------------------------------------------------------
void TMathUtil::GetBary(
				  Vector3 & pt
                , Vector3 & origin
				, Vector3 & U
				, Vector3 & V
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
float TMathUtil::GetRayToInfinityPlaneDistance(
				          const Ray & ray
				        , const Vector3 & pos
				        , const Vector3 & axis1
				        , const Vector3 & axis2
						, float * u
						, float * v
						, bool bothSide
				        )
{
	Vector3 p	= ray.getOrigin();

	Vector3 dir = ray.getDirection();

	//float length = dir.length();

	Vector3 mNormal = axis1.crossProduct( axis2 );
		
	float d = dir.dotProduct( mNormal );
	if( d == 0.0f )
		return -1.0f;
	

	Vector3 ap = pos - ray.getOrigin();

	float t = ap.dotProduct( mNormal );	

	t /= d;

	if( ! bothSide )
	{
		if( t < 0.0f )
			return -1.0f;
	}

	Vector3 tp = p + t * dir;

	Vector3 v0 = axis2;
	Vector3 v1 = axis1;
	Vector3 v2 = tp - pos;


	float dot00 = v0.dotProduct( v0 );
	float dot01 = v0.dotProduct( v1 );
	float dot02 = v0.dotProduct( v2 );
	float dot11 = v1.dotProduct( v1 );
	float dot12 = v1.dotProduct( v2 );

	float inv = 1.0f / ( dot00 * dot11 - dot01 * dot01);
	*u = ( dot11 * dot02 - dot01 * dot12 ) * inv;
	*v = ( dot00 * dot12 - dot01 * dot02 ) * inv;
		
	return t;		
}

float TMathUtil::GetRayToPlaneDistance(
                                const Ray & ray
                            ,   const Vector3 & pos
                            ,   const Vector3 & axis1
                            ,   const Vector3 & axis2
                            ,	float size
							,	bool bothSide
                            )
{
    Vector3 p	= ray.getOrigin();

	Vector3 dir = ray.getDirection();

	//float length = dir.length();

	Vector3 mNormal = axis1.crossProduct( axis2 );
		
	float d = dir.dotProduct( mNormal );
	if( d == 0.0f )
		return -1.0f;
	/*if( d >= 0.0f )
		return -1.0f;*/

	Vector3 ap = pos - ray.getOrigin();

	float t = ap.dotProduct( mNormal );	

	t /= d;

	if( ! bothSide )
	{
		if( t < 0.0f )
			return -1.0f;
	}

	Vector3 tp = p + t * dir;

	Vector3 v0 = axis2;
	Vector3 v1 = axis1;
	Vector3 v2 = tp - pos;


	float dot00 = v0.dotProduct( v0 );
	float dot01 = v0.dotProduct( v1 );
	float dot02 = v0.dotProduct( v2 );
	float dot11 = v1.dotProduct( v1 );
	float dot12 = v1.dotProduct( v2 );

	float inv = 1.0f / ( dot00 * dot11 - dot01 * dot01);
	float u = ( dot11 * dot02 - dot01 * dot12 ) * inv;
	float v = ( dot00 * dot12 - dot01 * dot02 ) * inv;
		
	if( u >= 0.0f && v >= 0.0f && u <= size && v <= size )
		return t;
	return -1.0f;
}

//------------------------------------------------------------------------------
float TMathUtil::SmoothStep(float a, float b, float x)
{
    // Scale, bias and saturate x to 0..1 range
    if( x < a)
		return 0.0f;
	if( x >= b )
		return 1.0f;
    x = (x - a) / (b - a); 
	return x*x*(3 - 2*x);
}


//------------------------------------------------------------------------------
bool TMathUtil::IsInside( const std::vector< Vector2 > & polygon,  const Vector2 &pt)
{
  int counter = 0;
  float x = pt.x;
  float y = pt.y;
  int size = (int)polygon.size();
  for(int i=0;i<size;i++)
  {
      int t1 = i;
      int t2 = (i+1) % polygon.size();
      const Vector2 p1 = polygon[t1];
      const Vector2 p2 = polygon[t2];

	  float maxX = p1.x >= p2.x ? p1.x : p2.x;
	  float minY = p1.y <= p2.y ? p1.y : p2.y;
	  float maxY = p1.y >= p2.y ? p1.y : p2.y;
	    

      if( y > minY ) // check y
      {
         if( y <= maxY )
         {
           if( x <= maxX )
           {
               if( p1.y != p2.y )
               {
                   float x_inter = (y-p1.y)*(p2.x-p1.x)/(p2.y-p1.y) + p1.x;
                   if( p1.x==p2.x || x <= x_inter )
                       counter ++;
               }
           }
         } 
      }
  }
  if( counter % 2 == 0 )
      return false;
  return true;
}

//------------------------------------------------------------------------------
int TMathUtil::GetNextPower2(int width)
{
	int b = width;
	int n;
	for (n = 0; b != 0; n++) 
		b >>= 1;
	b = 1 << n;
	if (b == 2 * width) 
		b >>= 1;
	return b;
}

//------------------------------------------------------------------------------
Vector3 TMathUtil::GetProjectedPoint(const Vector3 & from, const Vector3 & v, const Vector3 & pt)
{
	return from + ( v.dotProduct(pt-from) / v.dotProduct(v) ) * v; 
}

//------------------------------------------------------------------------------
void AssertIsNumber(float x)
{
	if( x != x )
	{
		assert( !"Is NOT a Number!!!" );
	}
}

//------------------------------------------------------------------------------