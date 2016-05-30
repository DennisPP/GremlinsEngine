#ifndef __FireEngine__Fonts__FontManager__h__
#define __FireEngine__Fonts__FontManager__h__

#include <map>

#include <string>

//--------------------------------------------------------------------------
class TBMFont;

//--------------------------------------------------------------------------
class TFontManager
{
public:
	
	static void Startup();
	static void Shutdown();

	TBMFont * GetFont(const std::string & name);

	static TFontManager * GetInstance();
	
private:
	TFontManager();
	~TFontManager();
	
	
	static  TFontManager * mInstance;

	std::map<std::string, TBMFont *> mFonts;	

};

//--------------------------------------------------------------------------
#endif