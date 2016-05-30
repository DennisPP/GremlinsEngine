#include "BitmapFont.h"

#include <string>
#include <cassert>


#include "Core/GremlinsFramework.h"
#include "Graphics/TextureManager.h"
#include "Graphics/FlatRenderer.h"

//#include "FontRenderer.h"


//#include "Graphics/Renderer.h"

#include "graphics/Quad2D.h"
//#include "graphics/Quad3D.h"


//------------------------------------------------------------------------------
TBMFont::TBMFont()
{
	mScale = 1.5f;
	mProportion = 1.0f;
	mSpacing = 1.0f;

	mTextureSize = 512;
}

//------------------------------------------------------------------------------
TBMFont::~TBMFont()
{
}

//------------------------------------------------------------------------------
void TBMFont::SetScale(float s) 
{
	mScale = s; 
}

//------------------------------------------------------------------------------
float TBMFont::GetStringWidth(const char *string) 
{
	int i;
	float linew, w = 0;

	while(*string)
	{
		linew = 0;

		while(*string && *string != '\n')
		{
			i=(unsigned char)*string;
			//if( ! letters[i] )
			//	i='?';
			//if(letters[i])
			i = mMap[i];

			TBMFont::TFontChar & ch = mFontChars[i];

			float fTracking = 0.0f;
			
			linew += ch.xAdv + fTracking;
			string++;
		}

		if(linew > w) 
			w = linew;

		while (*string == '\n' || *string == '\r') 
			string++;
	}

	return w * mScale * mProportion;
}


//------------------------------------------------------------------------------
float TBMFont::GetStringHeight(const char *string) 
{
	int i;
	float lineHeight, h = 0;

	while(*string)
	{
		lineHeight = 0;
		while(*string && *string != '\n')
		{
			i=(unsigned char)*string;
			//if( ! letters[i] )
			//	i='?';
			//if(letters[i])
			i = mMap[i];

			TBMFont::TFontChar & ch = mFontChars[i];

			//float fTracking = 0.0f;
			
			if( ch.hPos > lineHeight )
				lineHeight = ch.hPos;
			string++;
		}

		
		h += lineHeight;

		while (*string == '\n' || *string == '\r') 
			string++;
	}
	return h * mScale;
}


//------------------------------------------------------------------------------
void TBMFont::SetColor(const TColor & color)
{
	mColor = color;
}

//------------------------------------------------------------------------------
void TBMFont::Render(float x, float y, const char *string, int align, int sFactor, int dFactor)
{
#if 1
	int i;
	float	cursorX =x;
	TFlatRenderer* renderer = TFlatRenderer::GetInstance();
    
    
	
    

	renderer->BindTexture(mTexture);
	
	//JRenderer* renderer = JRenderer::GetInstance();

	align &= HGETEXT_HORZMASK;
	if(align==HGETEXT_RIGHT) 
		cursorX -= GetStringWidth(string);
	if(align==HGETEXT_CENTER) 
		cursorX -= int(GetStringWidth(string)/2.0f);

	while(*string)
	{
		if(*string=='\n')
		{
			y += int(mHeight * mScale * mSpacing);
			cursorX = x;
			if(align == HGETEXT_RIGHT)  
				cursorX -= GetStringWidth(string+1);
			if(align == HGETEXT_CENTER) 
				cursorX -= int(GetStringWidth(string+1)/2.0f);
		}
		else
		{
			i = mMap[*string];
			TBMFont::TFontChar & ch = mFontChars[i];
			float fTracking = 0.0f;

			////if( ! letters[i] )
			////	i='?';
			//if(letters[i])
			//{

			float w = ch.wPos * mScale * mProportion;
			float h = ch.hPos * mScale;

			float xx = cursorX + ch.xOffset * mScale * mProportion;

			float yOffset = ch.yOffset * mScale;
						
		
			float u0 = ch.x / (float)mTextureSize;
			float v1 = 1.0f - (ch.y + ch.hPos) / (float)mTextureSize;
			float u1 = (ch.x + ch.wPos) / (float)mTextureSize;
			float v0 = 1.0f - ch.y / (float)mTextureSize;
		
            TFlatRenderer::TVertexData pt0(xx, y+yOffset, 0.0f, u0,v0);
            TFlatRenderer::TVertexData pt1(xx + w, y+yOffset, 0.0f, u1,v0);
            TFlatRenderer::TVertexData pt2(xx + w, h + y+yOffset, 0.0f, u1,v1);
            TFlatRenderer::TVertexData pt3(xx, h + y+yOffset, 0.0f, u0,v1);
            renderer->RenderTriangle(pt0, pt1, pt2, mColor);
            renderer->RenderTriangle(pt0, pt2, pt3, mColor);
            
			cursorX += (ch.xAdv + fTracking)*mScale*mProportion;			
		}
		string++;
	}

	renderer->Flush();

	//glDisable(GL_BLEND);
    
#endif
}

void TBMFont::Render3D(const TBasis& b, const char* string)
{
	int align = HGETEXT_CENTER;
	int i;
	float x = 0.0f;
	float y = 0.0f;

	float	cursorX = x;
	TFlatRenderer* renderer = TFlatRenderer::GetInstance();
	
//	glEnable(GL_BLEND);

	//glBlendFunc(GL_ONE,GL_ONE);
	//glBlendFunc(sFactor, dFactor);


	renderer->BindTexture(mTexture);
	
	//JRenderer* renderer = JRenderer::GetInstance();

	align &= HGETEXT_HORZMASK;
	if(align==HGETEXT_RIGHT) 
		cursorX -= GetStringWidth(string);
	if(align==HGETEXT_CENTER) 
		cursorX -= int(GetStringWidth(string)/2.0f);

	while(*string)
	{
		if(*string=='\n')
		{
			y += int(mHeight * mScale * mSpacing);
			cursorX = x;
			if(align == HGETEXT_RIGHT)  
				cursorX -= GetStringWidth(string+1);
			if(align == HGETEXT_CENTER) 
				cursorX -= int(GetStringWidth(string+1)/2.0f);
		}
		else
		{
			i = mMap[*string];
			TBMFont::TFontChar & ch = mFontChars[i];
			float fTracking = 0.0f;

			////if( ! letters[i] )
			////	i='?';
			//if(letters[i])
			//{

			float w = ch.wPos * mScale * mProportion;
			float h = ch.hPos * mScale;

			float xx = cursorX + ch.xOffset * mScale * mProportion;

			float yOffset = ch.yOffset * mScale;
			
			TQuad2D q(0.0f,0.0f, w,h);		
			float u0 = ch.x / (float)mTextureSize;
			float v0 = 1.0f - (ch.y + ch.hPos) / (float)mTextureSize;
			float u1 = (ch.x + ch.wPos) / (float)mTextureSize;
			float v1 = 1.0f - ch.y / (float)mTextureSize;
			
			
			///float hw = 0.5f * w;
			///float hh = 0.5f * h;

			Vector3 p0 = b.P + (xx) * b.U + (y+yOffset)*b.V;
			Vector3 p1 = b.P + (xx+w) * b.U + (y+yOffset)*b.V;
			Vector3 p2 = b.P + (xx+w) * b.U + (y+yOffset+h)*b.V;
			Vector3 p3 = b.P + (xx) * b.U + (y+yOffset+h)*b.V;

			TFlatRenderer::TVertexData pt0(p0.x, p0.y, p0.z, u0, v0);
			TFlatRenderer::TVertexData pt1(p1.x, p1.y, p1.z, u1, v0);
			TFlatRenderer::TVertexData pt2(p2.x, p2.y, p2.z, u1, v1);
			TFlatRenderer::TVertexData pt3(p3.x, p3.y, p3.z, u0, v1);

		
			renderer->RenderTriangle(pt0, pt1, pt2);
			renderer->RenderTriangle(pt0, pt2, pt3);

			cursorX += (ch.xAdv + fTracking)*mScale*mProportion;			
		}
		string++;
	}

	renderer->Flush();
}

void TBMFont::GetFontChars(const char* str, std::vector<TFontChar>& chars)
{
	chars.clear();

	while(*str)
	{
		int i = mMap[*str];
		TBMFont::TFontChar & ch = mFontChars[i];
		chars.push_back(ch);			
		++ str;
	}
}

unsigned int TBMFont::GetTexture() const
{
	return mTexture;
}

int TBMFont::GetTextureSize() const
{
	return mTextureSize;
}