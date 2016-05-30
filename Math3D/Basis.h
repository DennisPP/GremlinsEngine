#ifndef __Basis__h__
#define __Basis__h__

#include "Vector3.h"			
#include "Matrix4.h"


//------------------------------------------------------------------------------
class TBasis
{
public:
	TBasis();
	TBasis(const ::Vector3 & p);

	void		Rotate(const Vector3 axis, float angle);
	Vector3		ToWorld(const Vector3 pt) const;
	Vector3		ToLocal(const Vector3 & pt) const;

	void		GetOrientation(Matrix4 & mat) const;
	Vector3		P, U, V, N;		
};

//------------------------------------------------------------------------------
#endif