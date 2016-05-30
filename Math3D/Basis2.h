#ifndef __Basis2__
#define __Basis2__

#include "Vector2.h"

class Basis2
{
public:
    
    Basis2(const Vector2& p = Vector2::ZERO, const Vector2& u = Vector2::UNIT_X, const Vector2& v = Vector2::UNIT_Y);
  
    Vector2 ToWorld(const Vector2& pt) const;
    Vector2 ToLocal(const Vector2& pt) const;
    
    
    void Rotate(const float angle);
    
    
    Vector2 P, U, V;
};


#endif