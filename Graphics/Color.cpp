#include "Color.h"
#include "Math3D/Math3D.h"

//------------------------------------------------------------------------------
TColor TColor::Black = TColor(0.0f,0.0f,0.0f,1.0f);
TColor TColor::White = TColor(1.0f,1.0f,1.0f,1.0f);
TColor TColor::Red = TColor(1.0f,0.0f,0.0f,1.0f);
TColor TColor::Green = TColor(0.0f,1.0f,0.0f,1.0f);
TColor TColor::Yellow = TColor(1.0f,1.0f,0.0f,1.0f);
TColor TColor::Blue	= TColor(0.0f,0.0f,1.0f,1.0f);
TColor TColor::LightGray = TColor(0.85f,0.85f, 0.85f, 1.0f);
TColor TColor::Orange = TColor(1.0f, 0.647, 0.0f, 1.0f);


TColor TColor::EditorBackgroundColor = TColor(0.2235294f, 0.2235294f,0.2235294f, 1.0f);

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TColor::TColor() : r( 1.0f ), g( 1.0f ), b( 1.0f ), a(1.0f) 
{
}

//------------------------------------------------------------------------------
TColor::TColor( float _r, float _g, float _b, float _a )
:	r( _r )
,	g( _g )
,	b( _b )
,	a( _a )
{
}

TColor TColor::FromRandomColor()
{
    float r = Math::RangeRandom(0.0f,1.0f);
    float g = Math::RangeRandom(0.0f,1.0f);
    float b = Math::RangeRandom(0.0f,1.0f);
    
    return TColor(r, g, b, 1.0f);
}


//------------------------------------------------------------------------------
TColor TColor::FromIntColor(int _r, int _g, int _b, int _a)
{
	float r = (float)_r / 255.0f;
	float g = (float)_g / 255.0f;
	float b = (float)_b / 255.0f;
	float a = (float)_a / 255.0f;
	return TColor(r,g,b,a);
}

//------------------------------------------------------------------------------
bool TColor::operator == (const TColor & color) const
{
	return color.r == r && color.g == g && color.b == b && color.a == a;
}

//------------------------------------------------------------------------------	
bool TColor::operator != (const TColor & color) const
{
	return ! (*this == color);
}

TColor TColor::operator + (const TColor& color) const
{
    TColor c;
    c.r = r + color.r;
    c.g = g + color.g;
    c.b = b + color.b;
    c.a = a + color.a;
    return c;
}

TColor TColor::operator - (const TColor& color) const
{
    TColor c;
    c.r = r - color.r;
    c.g = g - color.g;
    c.b = b - color.b;
    c.a = a - color.a;
    return c;
}

TColor TColor::operator * (float v) const
{
    TColor c;
    c.r = r * v;
    c.g = g * v;
    c.b = b * v;
    c.a = a * v;
    return c;
}



//------------------------------------------------------------------------------