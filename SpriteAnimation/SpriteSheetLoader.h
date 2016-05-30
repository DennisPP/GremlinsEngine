#ifndef __FireEngine__SpriteAnimation__SpriteSheetLoader__h__
#define __FireEngine__SpriteAnimation__SpriteSheetLoader__h__

#include <map>

class TSpriteSheet;

//------------------------------------------------------------------------------
class TSpriteSheetLoader
{
public:
	
	static TSpriteSheetLoader * GetInstance();


	TSpriteSheet * LoadSpriteSheet(const char *filename, const char * directory);
	static void Startup();
	static void Shutdown();

private:
	TSpriteSheetLoader();
	~TSpriteSheetLoader();
	
	std::map<const std::string, TSpriteSheet*> mSpriteSheets;

	static TSpriteSheetLoader * mInstance;
};

//------------------------------------------------------------------------------
#endif