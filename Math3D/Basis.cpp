#include "Basis.h"


//------------------------------------------------------------------------------
TBasis::TBasis()
:	P( Vector3::ZERO )
,	U( 1.0f, 0.0f, 0.0f )
,	V( 0.0f, 1.0f, 0.0f )
,	N( 0.0f, 0.0f, 1.0f )
{
}

//------------------------------------------------------------------------------
TBasis::TBasis(const Vector3 & pos)
:	P( pos )
,	U( 1.0f, 0.0f, 0.0f )
,	V( 0.0f, 1.0f, 0.0f )
,	N( 0.0f, 0.0f, 1.0f )
{	
}

//------------------------------------------------------------------------------
void TBasis::GetOrientation(Matrix4 & mat) const
{
	mat.SetColumn(0, Vector4( U.x,  U.y,  U.z, 0.0f));
	mat.SetColumn(1, Vector4( V.x,  V.y,  V.z, 0.0f));
	mat.SetColumn(2, Vector4( N.x,  N.y,  N.z, 0.0f));
	mat.SetColumn(3, Vector4(0.0f, 0.0f, 0.0f, 1.0f));


}

//------------------------------------------------------------------------------
void TBasis::Rotate( const Vector3 axis, float angle )
{
	Quaternion q( angle, axis );

	U = q * U;
	V = q * V;
	N = q * N;	
}


//------------------------------------------------------------------------------
Vector3 TBasis::ToLocal( const Vector3 & pt ) const
{
	Vector3 p = pt - P;
	Matrix3 mat;
	mat.SetColumn( 0, Vector3(U.x, V.x, N.x) );
	mat.SetColumn( 1, Vector3(U.y, V.y, N.y) );
	mat.SetColumn( 2, Vector3(U.z, V.z, N.z) );

	p = mat * p;
	return p;
}

//------------------------------------------------------------------------------
Vector3 TBasis::ToWorld( const Vector3 pt ) const
{
	return P + pt.x * U + pt.y * V + pt.z * N;
}

//------------------------------------------------------------------------------