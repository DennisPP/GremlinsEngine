#ifndef __Fire__Graphics__TextureManager__h__
#define __Fire__Graphics__TextureManager__h__

#include <map>
#include <string>


#include "GraphicsHeader.h"

#include "TextureInfo.h"


//------------------------------------------------------------------------------
class TTextureManager
{
public:

	static TTextureManager & GetInstance(); 	
	~TTextureManager();

	const TTextureInfo & GetTexture(const char* filePath);
	bool IsTextureLoaded(const char * filePath, GLuint & textureID);

	void UnloadTexture(const char * filePath);
	void UnloadTexture(GLuint textureID);
	void UnloadAllTextures();

	static void Startup();
	static void Shutdown();
    
    static bool IsExist(const char* path);

private:
	TTextureManager();	
		
private:
	static TTextureManager * mInstance;
	bool LoadTextureFromPNG(const char* file, GLuint * textureID, int * width, int * height);	
	std::map<std::string , TTextureInfo *> mTextureInfoMap;
};

//------------------------------------------------------------------------------
#endif