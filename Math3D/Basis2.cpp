#include "Basis2.h"


Basis2::Basis2(const Vector2& p, const Vector2& u, const Vector2& v)
:   P(p), U(u), V(v)
{
}

Vector2 Basis2::ToWorld(const Vector2& pt) const
{
    return P + U * pt.x + V * pt.y;
}


Vector2 Basis2::ToLocal(const Vector2& pt) const
{
    Vector2 d = pt - P;
        
    float x = d.dotProduct(U) / U.length();
    float y = d.dotProduct(V) / V.length();
    return Vector2(x,y);
}

void Basis2::Rotate(const float angle)
{
    U.RotateAround(Vector2::ZERO, angle);
    V.RotateAround(Vector2::ZERO, angle);
}
