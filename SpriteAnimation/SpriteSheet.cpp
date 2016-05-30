#include "SpriteSheet.h"

#include "SpriteAnimation.h"

#include <cassert>

//------------------------------------------------------------------------------
TSpriteSheet::TSpriteSheet()
:	mWidth(-1)
,	mHeight(-1)
{
}

//------------------------------------------------------------------------------
TSpriteSheet::~TSpriteSheet()
{
	std::map<std::string, TSprite *>::iterator it = mSprites.begin();
	for( ;it != mSprites.end();++it )
	{
		TSprite * sprite = it->second;
		if( sprite )
		{
			delete sprite;
		}
	}
}

//------------------------------------------------------------------------------
void TSpriteSheet::AddSprite(TSprite * sprite)
{
	mSprites[sprite->GetName()] = sprite;
}

//------------------------------------------------------------------------------
TSprite * TSpriteSheet::GetSprite(const std::string & name) const
{
	std::map<std::string, TSprite *>::const_iterator it = mSprites.find(name);
	return it == mSprites.end() ? NULL : it->second;
}

//------------------------------------------------------------------------------
TSpriteAnimation * TSpriteSheet::GetSpriteAnimation(const std::string & name, int rows, int columns)
{
	std::map<std::string, TSprite *>::const_iterator it = mSprites.find(name);
	if( it != mSprites.end() )
	{
		TSprite * sprite = it->second;
		std::vector<TSprite> subSprites;
		sprite->MakeSubSprites(subSprites,rows,columns);
		TSpriteAnimation * spriteAnimation = new TSpriteAnimation();
		for(size_t j=0;j<subSprites.size();++j)
		{
			TSpriteFrame frame;	
			frame.mSprite = subSprites[j];
			spriteAnimation->AddSpriteFrame(frame);		
		}

		return spriteAnimation;
	}
	return NULL;
}

//------------------------------------------------------------------------------
void TSpriteSheet::SetTexture(const TTextureInfo & texture)
{
	mTextureInfo = texture;
}

//------------------------------------------------------------------------------
void TSpriteSheet::SetSize(int width, int height)
{
	assert( width >= 0 );
	assert( height >= 0 );

	mWidth = width;
	mHeight = height;
}

//------------------------------------------------------------------------------
int TSpriteSheet::GetWidth() const
{
	return mWidth;
}

//------------------------------------------------------------------------------
int TSpriteSheet::GetHeight() const
{
	return mHeight;
}

//------------------------------------------------------------------------------