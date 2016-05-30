#ifndef __FireEngine__SpriteAnimation__SpriteSheet__h__
#define __FireEngine__SpriteAnimation__SpriteSheet__h__

#include "Graphics/TextureInfo.h"

#include <map>

class TSprite;
class TSpriteAnimation;

//------------------------------------------------------------------------------
class TSpriteSheet
{
	friend class TSpriteSheetLoader;
protected:
	~TSpriteSheet();	
public:
	TSpriteSheet();
	
	void SetSize(int width, int height);

	void SetTexture(const TTextureInfo & texture);

	int GetWidth() const;
	int GetHeight() const;

	void AddSprite(TSprite * sprite);

	TSprite * GetSprite(const std::string & name) const;

	TSpriteAnimation * GetSpriteAnimation(const std::string & name, int rows, int columns);

	int mWidth;
	int mHeight;

	std::map<std::string , TSprite *> mSprites;

	TTextureInfo mTextureInfo;



};

//------------------------------------------------------------------------------
#endif