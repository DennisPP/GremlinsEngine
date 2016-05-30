#ifndef _FONT_RENDERER_H_
#define _FONT_RENDERER_H_

#include "Graphics/Quad2D.h"
#include "Graphics/Color.h"

//------------------------------------------------------------------------------
class TShader;



//------------------------------------------------------------------------------
class TFontRenderer
{
private:
	TFontRenderer();	
public:

	static TFontRenderer & GetInstance();
	static TFontRenderer * mInstance;	
	static void Startup();
	static void Shutdown();

	~TFontRenderer();

	//void RenderQuad(TQuad2D * quad);
	void RenderQuad(TQuad2D * quad, float xc, float yc, float angle, bool flipped = false);
	void Reset();
	void Flush();
	void BindTexture(unsigned int texture);
	void SetColor(const TColor & color);
protected:
	void _LoadShaders();

	TShader * mShader;

	unsigned int mTexture;

	int mDiffuseLocation;
	int mColorLocation;
	TColor mColor;

	int mVertexCount;

	float * mVertices;
	float * mUVs;

	const int mMaxVertexCount;


	float * GetMVPMatrix();
	
	
	enum 
	{
		VertexArray = 0,
		TexCoordArray
	};
};

//------------------------------------------------------------------------------
#endif