#ifndef __Color__h__
#define __Color__h__

//------------------------------------------------------------------------------
class TColor
{
public:
	TColor();
	TColor( float _r, float _g, float _b, float _a );
	static TColor FromIntColor(int r, int g, int b, int a);
    static TColor FromRandomColor();

	bool operator == (const TColor & color) const;
	bool operator != (const TColor & color) const;

    TColor operator + (const TColor& color) const;
    TColor operator - (const TColor& color) const;

    TColor operator * (float v) const;

	
	float r, g, b, a;

	static TColor Black;
	static TColor White;
    static TColor Red;
	static TColor Green;
	static TColor Blue;
	static TColor Yellow;
	static TColor LightGray;
    static TColor Orange;
    
    
    static TColor EditorBackgroundColor;
};

//------------------------------------------------------------------------------
#endif