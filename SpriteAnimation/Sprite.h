#ifndef __FIREENGINE_GRAPHICS_SPRITE_H_
#define __FIREENGINE_GRAPHICS_SPRITE_H_

#include <string>
#include <vector>
#include "Graphics/Color.h"


//------------------------------------------------------------------------------
class TSprite
{				 
	friend class TSpriteSheet;
	friend class TSpriteSheetLoader;

	friend class TSpriteFrame;
	friend class TSpriteAnimation;

public:
	class TQuad
	{
	public:
		unsigned int mTexture;
		float mX, mY;
		float mWidth, mHeight;
		float mU0, mV0, mU1, mV1;
	};

protected:
public:
	TSprite();
	~TSprite();

public:
	
	void SetSize(float w, float h);
	void SetUVs(float u0, float v0, float u1, float v1);
	void SetTexture(unsigned int texture);

	void SetName(const std::string & name);
	const std::string & GetName() const;

	void MakeSubSprites(std::vector<TSprite> & subSprites, int rows, int columns);

    static TSprite* SpriteFromImage(const char* path);
    
public:
	TQuad mQuad;

protected:
	std::string mName;
};


void RenderSpriteEx(TSprite* sprite, float x, float y);

void RenderRotatedSprite(TSprite * sprite, float x, float y, float angle, float scaleX = 1.0f, float scaleY = 1.0f, const TColor& color = TColor::White);
void RenderScaledSprite(TSprite * sprite, float x, float y, float sx, float sy);
void RenderSprite(TSprite * sprite, float x, float y, float scale = 1.0f, const TColor & color = TColor::White);
void RenderSprite(TSprite * sprite, float x0, float y0, float x1, float y1);

//------------------------------------------------------------------------------

#endif