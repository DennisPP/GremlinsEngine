#ifndef __OBB2D__h__
#define __OBB2D__h__

#include "Math3D/Vector2.h"

class OBB2D 
{

public:
	OBB2D(const Vector2 & center, const Vector2 & u , const Vector2 & v, float halfwidth, float halfheight); 
    
	OBB2D(const Vector2 & center, float w, float h, float angle);
    void MoveTo(const Vector2 & center);
	bool Overlaps(const OBB2D & other) const;
	//void render() const ;
    Vector2        corner[4];
    
private:

   Vector2        axis[2];
   double          origin[2];

    bool Overlaps1Way(const OBB2D & other) const ;

    void ComputeAxes();
};


#endif