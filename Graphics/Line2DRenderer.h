#ifndef __FireEngine__Graphics__Line2DRenderer__h__
#define __FireEngine__Graphics__Line2DRenderer__h__

#include "Color.h"

#include "Graphics/Shader.h"

//------------------------------------------------------------------------------
class TLine2DRenderer
{
public:
	~TLine2DRenderer();
	static TLine2DRenderer * GetInstance();
	
	void RenderLine(float x0, float y0, float x1, float y1);
	void Reset();
	void Flush();

	void SetColor(const TColor & color);
	void SetViewportSize(float width, float height);
	void SetToDeviceSize();

	static void Startup();
	static void Shutdown();
	
    bool IsEnabled() const;
    void SetEnabled(bool b);
    
protected:

	static TLine2DRenderer * mInstance;

	TLine2DRenderer();
	
	void _LoadShaders();

	TShader * mShader;

    bool isEnabled;

	int mColorLocation;
	
	TColor mColor;

	int mVertexCount;

	float * mVertices;
	
	const int mMaxVertexCount;

	enum 
	{
		VertexArray = 0,
	};
};

//------------------------------------------------------------------------------
#endif
