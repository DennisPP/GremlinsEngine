#include "BMFontLoader.h"

#include "BitmapFont.h"

#include "Core/GremlinsFramework.h"
#include "Graphics/TextureManager.h"

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

//------------------------------------------------------------------------------
void TBMFontLoader::Load(const char* inputFileName, TBMFont * font)
{
	const std::string & root = TGremlinsFramework::GetInstance()->GetAssetRoot();
	
	
	char buff[1024] = "";
	int numChars = 0;


	font->mMap.clear();

	std::ifstream inputFile (inputFileName);

	

	if (inputFile.is_open())
	{
		inputFile.getline(buff, sizeof(buff)); //first line is not useful
		inputFile.getline(buff, sizeof(buff)); //second line is not useful
		// read some basic attributes
		int line_h, base, w, h, pages;
		if ( 5 != sscanf(buff, "common lineHeight=%d base=%d scaleW=%d scaleH=%d pages=%d",
			&line_h, &base, &w, &h, &pages) ) 
		{
//			ErrorPrintf("error reading in basic info\n");
			inputFile.close();
			assert(false);
			return;
		}
		assert(pages == 1);

		int id = 0;
		char texFileName[512];
		inputFile.getline(buff, sizeof(buff)); 
		if( 0 == sscanf(buff, "page id=%d file=\"%s", &id, texFileName) )
		{
			assert( false );
		}

		assert( strlen(texFileName) >= 1 );

		texFileName[strlen(texFileName)-1] = 0;
		std::string fullpath = root + std::string(texFileName);
		
        font->mTexture = TTextureManager::GetInstance().GetTexture(fullpath.c_str()).TextureID;
        
		//page id=0 file="chinese_00.png"
		inputFile.getline(buff, sizeof(buff)); //chars count = ????
		unsigned short charItr = 0;
		if( 1 != sscanf(buff, "chars count=%d", &numChars) )
		{
			//can't find num chars
			assert(false);
		}
		assert (numChars > 0);
		assert (numChars < 0xFFFF);

		//CUFTExporter fontExporter;	
		//fontExporter.Init(base, line_h, w, h, inputTextureName);

		while ( ! inputFile.eof() )
		{
			//read each line which corresponds to each char
			int id, x, y, charW, charH, x_ofs, y_ofs, x_advance, page, chnl;
			inputFile.getline(buff, sizeof(buff));
			if ( 10 == sscanf(buff, "char id=%d x=%d y=%d width=%d height=%d xoffset=%d yoffset=%d xadvance=%d page=%d chnl=%d",
					&id, 
					&x,
					&y,
					&charW,
					&charH,
					&x_ofs,
					&y_ofs,
					&x_advance,
					&page,
					&chnl 
					) )
			{
				TBMFont::TFontChar ulChar;
				ulChar.hPos = charH;
				ulChar.wPos = charW;
				ulChar.xOffset = x_ofs;
				ulChar.yOffset = y_ofs;
				ulChar.xAdv = x_advance;
				ulChar.x = x;
				ulChar.y = y;
			//	fontExporter.AddWChar((wchar)id, ulChar);
				
				font->mFontChars.push_back(ulChar);
				font->mMap[id] = (int)font->mFontChars.size() - 1;
				

				if(charH > font->mHeight) 
					font->mHeight = (float)charH;

				assert(page == 0);
				assert(charItr <= numChars);
				charItr++;
			}
		}

		assert(charItr == numChars);
		inputFile.close();

		//fontExporter.Export(outputFileName);
	}
}

//------------------------------------------------------------------------------