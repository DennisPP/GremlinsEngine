#include "Vector3.h"
#include "Math.h"

//------------------------------------------------------------------------------
const Vector3 Vector3::UnitScale( 1.0f, 1.0f, 1.0f );

const Vector3 Vector3::ZERO( 0, 0, 0 );

const Vector3 Vector3::UNIT_X( 1, 0, 0 );
const Vector3 Vector3::UNIT_Y( 0, 1, 0 );
const Vector3 Vector3::UNIT_Z( 0, 0, 1 );
const Vector3 Vector3::NEGATIVE_UNIT_X( -1,  0,  0 );
const Vector3 Vector3::NEGATIVE_UNIT_Y(  0, -1,  0 );
const Vector3 Vector3::NEGATIVE_UNIT_Z(  0,  0, -1 );

//------------------------------------------------------------------------------
Vector3 Vector3::GenerateOrthogonalVector() const
{
	Vector3 ret;
	if( fabs(x) <= fabs(y) && fabs(x) <= fabs(z) )
	{
		ret = Vector3( x, - z, y );
	}
	else
	if( fabs(y) <= fabs(z) && fabs(y) <= fabs( x) )
	{
		ret = Vector3( - z, y, x );
	}
	else		
	{
		ret = Vector3( - y, x, z );
	}
	Vector3 n = crossProduct( ret ).normalisedCopy();
	ret = n.crossProduct( *this ).normalisedCopy();
	return ret;
}

//------------------------------------------------------------------------------
float Vector3::GetSignedAngle(const Vector3& v0, const Vector3& v1)
{
	Vector3 vv0 = v0.normalisedCopy();
	Vector3 vv1 = v1.normalisedCopy();

	float dot = vv0.dotProduct( vv1 );

	if( dot < -1.0f )
		dot = -1.0f;
	if( dot > 1.0f )
		dot = 1.0f;
	float angle = acos( dot );

	if( vv0.crossProduct( vv1 ).z < 0.0f )
	{
		angle = - angle;
	}
	return angle;
}

//------------------------------------------------------------------------------