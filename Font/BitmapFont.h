
#ifndef FireEngine__Fonts__BitmapFont__h__
#define FireEngine__Fonts__BitmapFont__h__

#include "Graphics/Color.h"
#include "Graphics/Graphics.h"
#include "Graphics/TextureManager.h"

#include "Math3D/Basis.h"


#include <vector>
#include <map>


#define HGETEXT_LEFT		0
#define HGETEXT_RIGHT		1
#define HGETEXT_CENTER		2
#define HGETEXT_HORZMASK	0x03

#define HGETEXT_TOP			0
#define HGETEXT_BOTTOM		4
#define HGETEXT_MIDDLE		8
#define HGETEXT_VERTMASK	0x0C

//------------------------------------------------------------------------------
class TBMFont
{
	friend class TBMFontLoader;
public:
	typedef struct TFontChar
	{
		unsigned short		x;
		unsigned short		y;
		unsigned char		wPos;
		unsigned char		hPos;
		char				xOffset;
		char				yOffset;
		char				xAdv;
	} TFontChar;


public:
	TBMFont();
	~TBMFont();

	
	void Render(float x, float y, const char *string, int align = HGETEXT_LEFT, int sFactor = GL_SRC_ALPHA, int dFactor = GL_ONE_MINUS_SRC_ALPHA);
	void Render3D(const TBasis& b, const char* string);
	float GetStringWidth(const char *string);
	float GetStringHeight(const char *string);
	void SetScale(float s);

	void SetColor(const TColor & color);

	void GetFontChars(const char* str, std::vector<TFontChar>& chars);
	unsigned int GetTexture() const;
	int GetTextureSize() const;

protected:

	float		mScale;
	float		mProportion;
	float		mSpacing;
	float		mHeight;

	int mTextureSize;

	std::vector<TFontChar> mFontChars;
	std::map<int, int> mMap;
	unsigned int mTexture;
	TColor mColor;
};


//------------------------------------------------------------------------------
#endif
