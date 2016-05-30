#include "SpriteSheetLoader.h"

#include "SpriteAnimation.h"
#include "SpriteSheet.h"

#include "tinyxml.h"

#include "Graphics/TextureManager.h"



#include <cassert>

TSpriteSheetLoader * TSpriteSheetLoader::mInstance = NULL;

//------------------------------------------------------------------------------
TSpriteSheetLoader * TSpriteSheetLoader::GetInstance()
{
	assert( mInstance );
	return mInstance;
}

//------------------------------------------------------------------------------
TSpriteSheetLoader::TSpriteSheetLoader()
{
}

//------------------------------------------------------------------------------
TSpriteSheetLoader::~TSpriteSheetLoader()
{
	std::map<const std::string, TSpriteSheet*>::iterator it = mSpriteSheets.begin();
	for(; it!=mSpriteSheets.end();++it)
	{
		TSpriteSheet* s = it->second;
		if( s )
		{
			delete s;
		}		
	}
	mSpriteSheets.clear();
}
	

//------------------------------------------------------------------------------
void TSpriteSheetLoader::Startup()
{
	if( ! mInstance )
		mInstance = new TSpriteSheetLoader();
}

//------------------------------------------------------------------------------
void TSpriteSheetLoader::Shutdown()
{
	if( mInstance )
	{
		delete mInstance;
		mInstance = NULL;
	}
}



//------------------------------------------------------------------------------
TSpriteSheet * TSpriteSheetLoader::LoadSpriteSheet(const char *filename, const char * directory)
{
	char fullPath[256];
	sprintf(fullPath,"%s%s",directory,filename);

	std::map<const std::string, TSpriteSheet*>::iterator it 
		= mSpriteSheets.find(std::string(fullPath)); 
	if( it != mSpriteSheets.end() )
	{
		return it->second;
	}

	
	//FILE * fp = fopen(fullPath, "rb");
	//fseek(fp, 0, SEEK_END);
	//long fileSize = ftell(fp);
	//fseek(fp, 0, SEEK_SET);

	////int size = fileSystem->GetFileSize();
	//char *xmlBuffer = new char[fileSize+1];
	//fread(xmlBuffer,fileSize,1, fp);
	//fclose(fp);


	//xmlBuffer[fileSize] = 0;

	TiXmlDocument doc;
	bool loaded = doc.LoadFile(fullPath);
    assert( loaded );
	//doc.Parse(xmlBuffer);
	//delete[] xmlBuffer;


	TiXmlElement* resource = 0;
	
	TSpriteSheet * spriteSheet = NULL;
	resource = doc.FirstChildElement("TextureAtlas"); 
	if( resource )
	{
		spriteSheet = new TSpriteSheet();

		//const char * imagePath = resource->Attribute("imagePath");
		//assert( imagePath );
		int sheetWidth = 0;
		int sheetHeight = 0;
		bool hasWidth = TIXML_SUCCESS == resource->QueryIntAttribute("width", &sheetWidth);
		bool hasHeight =  TIXML_SUCCESS == resource->QueryIntAttribute("height", &sheetHeight);
		if( hasWidth && hasHeight )
		{
			spriteSheet->SetSize(sheetWidth,sheetHeight);
		}

		
		
		TiXmlElement * spriteElement = resource->FirstChildElement("sprite");
		for(;spriteElement;spriteElement=spriteElement->NextSiblingElement("sprite"))
		{
			int x = 0, y = 0, w = 0, h = 0;
			bool hasX = TIXML_SUCCESS == spriteElement->QueryIntAttribute("x", &x);
			bool hasY = TIXML_SUCCESS == spriteElement->QueryIntAttribute("y", &y);
			bool hasW = TIXML_SUCCESS == spriteElement->QueryIntAttribute("w", &w);
			bool hasH = TIXML_SUCCESS == spriteElement->QueryIntAttribute("h", &h);
					
			if( hasX && hasY && hasW && hasH )
			{
				const char * sheetName = spriteElement->Attribute("src");
				const char * name = spriteElement->Attribute("n");


				char buf[256];
				sprintf(buf,"%s%s",directory,sheetName);
				//printf("full path %s\n", buf);
				const TTextureInfo & texture = TTextureManager::GetInstance().GetTexture(buf);

				float u0 = (float)x / (float)sheetWidth;
				float v0 = (float)(y+h) / (float)sheetHeight;				
				float u1 = (float)(x+w) / (float)sheetWidth;
				float v1 = (float)y / (float)sheetHeight;
				
				
				TSprite * sprite = new TSprite();			
				sprite->SetSize((float)w, (float)h);
				sprite->SetUVs(u0, 1.0f - v0, u1, 1.0f - v1);					
				sprite->SetTexture(texture.TextureID);

				if( name )
					sprite->SetName(name);				
				spriteSheet->AddSprite(sprite);
			}
		}
	}

	

	


	
	mSpriteSheets.insert(std::pair<const std::string, TSpriteSheet*>(std::string(fullPath), spriteSheet));

	return spriteSheet;
}

//------------------------------------------------------------------------------