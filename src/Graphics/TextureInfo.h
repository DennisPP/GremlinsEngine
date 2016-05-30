#ifndef FireEngine__Graphics__TextureInfo__h__
#define FireEngine__Graphics__TextureInfo__h__

#include <string>

//------------------------------------------------------------------------------
class TTextureInfo
{
public:
	TTextureInfo();
		
	int	ImageWidth;
	int ImageHeight;
	unsigned int TextureID;
	std::string Name;
};


//------------------------------------------------------------------------------
#endif