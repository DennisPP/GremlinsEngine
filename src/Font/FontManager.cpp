#include "FontManager.h"

#include "Core/GremlinsFramework.h"

#include "BitmapFont.h"
#include "BMFontLoader.h"

#include <cassert>

//--------------------------------------------------------------------------
TFontManager * TFontManager::mInstance = 0;

//--------------------------------------------------------------------------
TFontManager::TFontManager()
{
    const std::string & root = TGremlinsFramework::GetInstance()->GetAssetRoot();
 

	std::string fontPath = root + "media/fonts/BigCaslon.fnt";
	TBMFont *font = new TBMFont();
	font->SetScale(0.5f); 
	TBMFontLoader::Load(fontPath.c_str(),font);	

	mFonts[fontPath] = font;
}

//--------------------------------------------------------------------------
TFontManager::~TFontManager()
{
	std::map<std::string, TBMFont *>::iterator it = mFonts.begin();
	for(;it!=mFonts.end();++it)
	{
		delete it->second;
	}
	mFonts.clear();
}

//--------------------------------------------------------------------------
void TFontManager::Startup()
{
	mInstance = new TFontManager();
	
}

//--------------------------------------------------------------------------
TBMFont * TFontManager::GetFont(const std::string & path)
{
	std::map<std::string, TBMFont *>::const_iterator it = mFonts.find(path);
	if( it == mFonts.end() )
	{
		TBMFont *font = new TBMFont();
		font->SetScale(0.5f); 
		TBMFontLoader::Load(path.c_str(),font);	
		mFonts[path] = font;	
		return font;
	}
	return it->second;
}

//--------------------------------------------------------------------------
void TFontManager::Shutdown()
{
	if( mInstance )
	{
		delete mInstance;
		mInstance = 0;
	}
}

//--------------------------------------------------------------------------
TFontManager * TFontManager::GetInstance()
{
	if( ! mInstance )
	{
		mInstance = new TFontManager();
	}
	return mInstance;
}

//-------------------------------------------------------------------------- 