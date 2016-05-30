#include "Sprite.h"

#include "Graphics/FlatRenderer.h"
#include "Graphics/TextureManager.h"

#include <cassert>

//------------------------------------------------------------------------------
TSprite::TSprite()
{
}

//------------------------------------------------------------------------------
TSprite::~TSprite()
{
}

//------------------------------------------------------------------------------
void TSprite::SetSize(float w, float h)
{
	mQuad.mWidth = w;
	mQuad.mHeight = h;
}

//------------------------------------------------------------------------------
void TSprite::SetUVs(float u0, float v0, float u1, float v1)
{
	mQuad.mU0 = u0;
	mQuad.mV0 = v0;
	mQuad.mU1 = u1;
	mQuad.mV1 = v1;
}

//------------------------------------------------------------------------------
void TSprite::SetTexture(unsigned int texture)
{
	mQuad.mTexture = texture;
}

//------------------------------------------------------------------------------
void TSprite::SetName(const std::string & name)
{
	mName = name;
}
	
//------------------------------------------------------------------------------
const std::string & TSprite::GetName() const
{
	return mName;
}

//------------------------------------------------------------------------------
void TSprite::MakeSubSprites(std::vector<TSprite> & subSprites, int rows, int columns)
{
	subSprites.clear();
	assert( rows > 0 && columns > 0 );

	float u0 = mQuad.mU0;
	float v0 = mQuad.mV0;
	float u1 = mQuad.mU1;
	float v1 = mQuad.mV1;

	float w = mQuad.mWidth / columns;
	float h = mQuad.mHeight / rows;

	float u_inc = (u1-u0) / columns;
	float v_inc = (v1-v0) / rows;	
	float v = v0;
	for(int j=0;j<rows;++j)
	{
		float u = u0;
		for(int i=0;i<columns;++i)
		{
			TSprite sprite;
			sprite.SetSize(w,h);
			sprite.SetUVs(u,v,u+u_inc,v+v_inc);
			sprite.SetTexture(mQuad.mTexture);
			subSprites.push_back(sprite);
			u += u_inc;	
		}
		v += v_inc;
	}
}


void RenderRotatedSprite(TSprite * sprite, float x, float y, float angle, float scaleX, float scaleY, const TColor& color)
{
	TFlatRenderer * flatRenderer = TFlatRenderer::GetInstance();
	flatRenderer->BindTexture(sprite->mQuad.mTexture);
	//flatRenderer->SetColor(color);
	float hw = scaleX * 0.5f * sprite->mQuad.mWidth;
	float hh = scaleY * 0.5f * sprite->mQuad.mHeight;

	Vector3 pts[4];
	pts[0] = Vector3(- hw, + hh, 0.0f);
	pts[1] = Vector3(+ hw, + hh, 0.0f);
	pts[2] = Vector3(+ hw, - hh, 0.0f);
	pts[3] = Vector3(- hw, - hh, 0.0f);

	if( angle != 0.0f )
	{
		for(int j=0;j<4;++j)
		{
			float xx = pts[j].x;
			float yy = pts[j].y;
			pts[j].x = (xx) * cos(angle) - (yy) * sin(angle);
			pts[j].y = (xx) * sin(angle) + (yy) * cos(angle);
		}
	}

	for(int j=0;j<4;++j)
	{
		pts[j].x += x;
		pts[j].y += y;
	}

	float u0 = sprite->mQuad.mU0;
	float v0 = sprite->mQuad.mV0;
	float u1 = sprite->mQuad.mU1;
	float v1 = sprite->mQuad.mV1;
		
	TFlatRenderer::TVertexData pt0(pts[0].x, pts[0].y, pts[0].z, u0, v0);
	TFlatRenderer::TVertexData pt1(pts[1].x, pts[1].y, pts[1].z, u1, v0);
	TFlatRenderer::TVertexData pt2(pts[2].x, pts[2].y, pts[2].z, u1, v1);
	TFlatRenderer::TVertexData pt3(pts[3].x, pts[3].y, pts[3].z, u0, v1);
	flatRenderer->RenderTriangle(pt0,pt1,pt2, color);
	flatRenderer->RenderTriangle(pt0,pt2,pt3, color);

	flatRenderer->Flush();
}

//------------------------------------------------------------------------------
void RenderSprite(TSprite * sprite, float x, float y, float scale, const TColor & color)
{
    
    
    const float w = scale * sprite->mQuad.mWidth;
    const float h = scale * sprite->mQuad.mHeight;
    
    const float hw = 0.5f * w;	
    const float hh = 0.5f * h;

    
	TFlatRenderer * flatRenderer = TFlatRenderer::GetInstance();
	flatRenderer->BindTexture(sprite->mQuad.mTexture);
    
    Vector3 p0(x - hw, y + hh, 0.0f);
	Vector3 p1(x + hw, y + hh, 0.0f);
	Vector3 p2(x + hw, y - hh, 0.0f);
	Vector3 p3(x - hw, y - hh, 0.0f);
    
	float u0 = sprite->mQuad.mU0;
	float v0 = sprite->mQuad.mV0;
	float u1 = sprite->mQuad.mU1;
	float v1 = sprite->mQuad.mV1;
    
	TFlatRenderer::TVertexData pt0(p0.x, p0.y, p0.z, u0, v0);
	TFlatRenderer::TVertexData pt1(p1.x, p1.y, p1.z, u1, v0);
	TFlatRenderer::TVertexData pt2(p2.x, p2.y, p2.z, u1, v1);
	TFlatRenderer::TVertexData pt3(p3.x, p3.y, p3.z, u0, v1);
	flatRenderer->RenderTriangle(pt0,pt1,pt2, color);
	flatRenderer->RenderTriangle(pt0,pt2,pt3, color);
}

//------------------------------------------------------------------------------
void RenderScaledSprite(TSprite * sprite, float x, float y, float sx, float sy)
{
	TFlatRenderer * flatRenderer = TFlatRenderer::GetInstance();
	flatRenderer->BindTexture(sprite->mQuad.mTexture);
	float hw = sx * 0.5f * sprite->mQuad.mWidth;
	float hh = sy * 0.5f * sprite->mQuad.mHeight;

	Vector3 p0(x - hw, y + hh, 0.0f);
	Vector3 p1(x + hw, y + hh, 0.0f);
	Vector3 p2(x + hw, y - hh, 0.0f);
	Vector3 p3(x - hw, y - hh, 0.0f);

	float u0 = sprite->mQuad.mU0;
	float v0 = sprite->mQuad.mV0;
	float u1 = sprite->mQuad.mU1;
	float v1 = sprite->mQuad.mV1;
		
	TFlatRenderer::TVertexData pt0(p0.x, p0.y, p0.z, u0, v0);
	TFlatRenderer::TVertexData pt1(p1.x, p1.y, p1.z, u1, v0);
	TFlatRenderer::TVertexData pt2(p2.x, p2.y, p2.z, u1, v1);
	TFlatRenderer::TVertexData pt3(p3.x, p3.y, p3.z, u0, v1);
	flatRenderer->RenderTriangle(pt0,pt1,pt2);
	flatRenderer->RenderTriangle(pt0,pt2,pt3);

	flatRenderer->Flush();
	
}


//------------------------------------------------------------------------------
void RenderSprite(TSprite * sprite, float x0, float y0, float x1, float y1)
{
	TFlatRenderer * flatRenderer = TFlatRenderer::GetInstance();
	flatRenderer->BindTexture(sprite->mQuad.mTexture);
	
	
	Vector3 p0(x0, y1, 0.0f);
	Vector3 p1(x1, y1, 0.0f);
	Vector3 p2(x1, y0, 0.0f);
	Vector3 p3(x0, y0, 0.0f);

	float u0 = sprite->mQuad.mU0;
	float v0 = sprite->mQuad.mV0;
	float u1 = sprite->mQuad.mU1;
	float v1 = sprite->mQuad.mV1;
		
	TFlatRenderer::TVertexData pt0(p0.x, p0.y, p0.z, u0, v0);
	TFlatRenderer::TVertexData pt1(p1.x, p1.y, p1.z, u1, v0);
	TFlatRenderer::TVertexData pt2(p2.x, p2.y, p2.z, u1, v1);
	TFlatRenderer::TVertexData pt3(p3.x, p3.y, p3.z, u0, v1);


    flatRenderer->RenderTriangle(pt0,pt1,pt2, TColor::White);
    flatRenderer->RenderTriangle(pt0,pt2,pt3, TColor::White);

	flatRenderer->Flush();
	
}

TSprite* TSprite::SpriteFromImage(const char* path)
{
    TTextureInfo info = TTextureManager::GetInstance().GetTexture(path);
    
    TSprite* sprite = new TSprite();
    sprite->mQuad.mX = 0.0f;
    sprite->mQuad.mY = 0.0f;
    sprite->mQuad.mU0 = 0.0f;
    sprite->mQuad.mV0 = 0.0f;
    sprite->mQuad.mU1 = 1.0f;
    sprite->mQuad.mV1 = 1.0f;
    sprite->mQuad.mWidth = info.ImageWidth;
    sprite->mQuad.mHeight = info.ImageHeight;
    sprite->mQuad.mTexture = info.TextureID;
    return sprite;
}



//------------------------------------------------------------------------------