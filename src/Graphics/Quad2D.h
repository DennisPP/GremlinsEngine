#ifndef __FireEngine__Graphics__Quad2D__h__
#define __FireEngine__Graphics__Quad2D__h__

//------------------------------------------------------------------------------
class TQuad2D
{
	public:
	TQuad2D();
	TQuad2D(float x, float y, float w, float h);

	unsigned int mTexture;
	float mX, mY;
	float mWidth, mHeight;
	float mU0, mV0, mU1, mV1;
};


//------------------------------------------------------------------------------
#endif