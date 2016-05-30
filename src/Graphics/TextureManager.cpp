#include <assert.h>


#if WIN32

#else
#include <unistd.h>
#endif


#include "TextureManager.h"

//#ifdef WIN32
#include "pngloadImage.h"

#include "TGALoader.h"

//#endif

TTextureManager * TTextureManager::mInstance = 0;

//------------------------------------------------------------------------------
TTextureManager::TTextureManager()
{
}

//------------------------------------------------------------------------------
TTextureManager::~TTextureManager()
{
	UnloadAllTextures();
}

//------------------------------------------------------------------------------
TTextureManager & TTextureManager::GetInstance()
{
	if( ! mInstance )
	{
		mInstance = new TTextureManager();
	}
	return * mInstance;
}

//------------------------------------------------------------------------------
void TTextureManager::Startup()
{
	mInstance = new TTextureManager();
}

//------------------------------------------------------------------------------
void TTextureManager::Shutdown()
{
	if( mInstance )
	{
		delete mInstance;
		mInstance = NULL;
	}
}

bool isPowerOfTwo(unsigned int x)
{
 return ((x & (x - 1)) == 0);
}

bool TTextureManager::IsExist(const char* path)
{
    return -1 != access(path, F_OK );
}

//------------------------------------------------------------------------------
const TTextureInfo & TTextureManager::GetTexture(const char* filePath)
{
	GLuint textureID;	
	if ( IsTextureLoaded(filePath, textureID) )
		return * mTextureInfoMap[std::string(filePath)];
	
	// Check File format 
	std::string filename = filePath;
	int dotPosition = (int)filename.find_last_of('.');
	assert(dotPosition != std::string::npos);
	std::string filetype = filename.substr(dotPosition+1);
    
		
	bool loaded = false;

	int width = -1;
	int height = -1;
    
    //printf("Loading:%s\n",filePath);
				
	if( 0 == strcmp(filetype.c_str(),"png") )
	{	
		loaded = LoadTextureFromPNG(filename.c_str(), &textureID, & width, & height);
	}
    else if( 0 == strcmp(filetype.c_str(), "tga") )
    {
        Texture texture;
        loaded = LoadTGA(&texture, filePath);
        if( loaded )
        {
            textureID = texture.texID;
            width = texture.width;
            height = texture.height;
        }
    }
            
		
	printf("Texture:%s \t Loaded:%s\n",filePath,loaded ? "YES":"NO");
		
	assert(loaded);
	assert( width > 0 && height > 0 );
	

	
	TTextureInfo * info = new TTextureInfo();
	info->Name = filename;
	info->TextureID = textureID;
	info->ImageWidth = width;
	info->ImageHeight = height;
	mTextureInfoMap[ filePath ] = info;
	return * info;	
}

//------------------------------------------------------------------------------
bool TTextureManager::IsTextureLoaded(const char* filePath, GLuint& textureID)
{
	std::map<std::string, TTextureInfo*>::iterator it = mTextureInfoMap.begin();
	for (;it != mTextureInfoMap.end();  ++it)
	{
		TTextureInfo * info = it->second;
		if (strcmp(filePath, info->Name.c_str()) == 0)
		{
			textureID = info->TextureID;
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------------
void TTextureManager::UnloadTexture(const char* filePath)
{
	std::map<std::string, TTextureInfo*>::iterator it = mTextureInfoMap.begin();	
	for (;it!=mTextureInfoMap.end(); ++it)
	{
		TTextureInfo * info = it->second;
		if ( 0 == strcmp(filePath, info->Name.c_str()) )
		{			
			glDeleteTextures(1, &(info->TextureID));
			if( info )
			{
				delete info;
			}			
			mTextureInfoMap.erase(it);		
			
			return;
		}
	}
}


//------------------------------------------------------------------------------
void TTextureManager::UnloadAllTextures()
{
	std::map<std::string, TTextureInfo*>::iterator it = mTextureInfoMap.begin();	
	for (; it != mTextureInfoMap.end(); ++it)
	{
		TTextureInfo * info = it->second;
		glDeleteTextures(1, &(info->TextureID));

		if( info )
		{
			delete info;
		}
	}
	mTextureInfoMap.clear();
}

//------------------------------------------------------------------------------
bool TTextureManager::LoadTextureFromPNG(const char* file, GLuint* textureID, int * outWidth, int * outHeight) 
{	

	int width, height;
	bool hasAlpha = true;
	unsigned char* bytes;
    
	if( loadPngImage(file, width, height, hasAlpha, &bytes) ) 
	{		
		GLenum errCode = glGetError();		
		if(errCode != GL_NO_ERROR)
		{
			//printf("%s\n", gluErrorString(errCode) );
			return false;
		}
		* outWidth = width;
		* outHeight = height;

		

		errCode = glGetError();		
		if(errCode != GL_NO_ERROR)
		{
			//printf("%s\n", gluErrorString(errCode) );
			return false;
		}


		GLuint imageID;
		glGenTextures ( 1, &imageID );
				

		glBindTexture ( GL_TEXTURE_2D, imageID );		
			
		errCode = glGetError();		
		if(errCode != GL_NO_ERROR)
		{
			//printf("%s\n", gluErrorString(errCode) );
			return false;
		}
        
        
        
		glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		
		// Auto gen mipmap.
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//		glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

		errCode = glGetError();		
		if(errCode != GL_NO_ERROR)
		{
			//printf("%s\n", gluErrorString(errCode) );
			return false;
		}
		if( hasAlpha )
		{			
			
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes );
			GLenum errCode = glGetError();		
			if(errCode != GL_NO_ERROR)
			{
				//printf("%s\n", gluErrorString(errCode) );
				return false;
			}
		}
		else		
		{
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes );
			GLenum errCode = glGetError();		
			if(errCode != GL_NO_ERROR)
			{
				//printf("%s\n", gluErrorString(errCode) );
				return false;
			}
		}
		glBindTexture( GL_TEXTURE_2D, 0);
		
		delete[] (unsigned char*) bytes;
		
		errCode = glGetError();		
		if(errCode != GL_NO_ERROR)
		{
			//printf("%s\n", gluErrorString(errCode) );
			return false;
		}

		*textureID = imageID;		
		
		return true;		
	}
	return false;

    
}

//------------------------------------------------------------------------------