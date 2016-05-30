#include "Line2DRenderer.h"


#include <cassert>

#include "TextureInfo.h"
#include "TextureManager.h"

#include "Core/Device.h"

#include "Core/GremlinsFramework.h"

#include "Shader.h"

TLine2DRenderer * TLine2DRenderer::mInstance = NULL; 

//------------------------------------------------------------------------------
TLine2DRenderer * TLine2DRenderer::GetInstance()
{
	if( ! mInstance )
	{
		mInstance = new TLine2DRenderer();
	}
	return mInstance;
}
	
//------------------------------------------------------------------------------
TLine2DRenderer::TLine2DRenderer()
:	mMaxVertexCount(128)
,	mColor(TColor::Yellow)
{	
	mVertexCount = 0;
	mVertices = new float[mMaxVertexCount<<1];
	
	_LoadShaders();
    
    isEnabled = true;
}

//------------------------------------------------------------------------------
void TLine2DRenderer::Startup()
{
	mInstance = new TLine2DRenderer();
}

//------------------------------------------------------------------------------
void TLine2DRenderer::Shutdown()
{
	if( mInstance )
	{
		delete mInstance;
		mInstance = NULL;
	}
}

bool TLine2DRenderer::IsEnabled() const
{
    return isEnabled;
}

void TLine2DRenderer::SetEnabled(bool b)
{
    isEnabled = b;
}

//------------------------------------------------------------------------------
void TLine2DRenderer::_LoadShaders()
{
	
    int mScreenWidth = Device::GetInstance()->renderWidth;
    int mScreenHeight = Device::GetInstance()->renderHeight;
    
    //mScreenWidth = 320;
    //mScreenHeight = 480;
    
    const std::string & root = TGremlinsFramework::GetInstance()->GetAssetRoot();
    
    
    printf("Root : %s\n", root.c_str());
    
    
	char vs[512];
	char fs[512];
	


	sprintf(vs, "%s%s", root.c_str(), "media/shaders/flat.vs");
	sprintf(fs, "%s%s", root.c_str(), "media/shaders/flat.fs");

    
    
    printf("Shader : %s\n", vs);
    printf("Shader : %s\n", fs);
    
	mShader = new TShader();	
	mShader->Compile(vs,fs);

	GLuint programId = mShader->GetProgram();
	glBindAttribLocation(programId, TLine2DRenderer::VertexArray, "vertex");
	
	mShader->LinkProgram();
	
	
	mColorLocation = glGetUniformLocation(programId,"color");

	int matrixIdx = glGetUniformLocation(programId, "MVPMatrix");
		
	mShader->Bind();

	glUniform4f(mColorLocation, mColor.r, mColor.g, mColor.b, mColor.a);
	
	const float sx = 2.0f / mScreenWidth;
	const float sy = 2.0f / mScreenHeight;	

	const float matrix[] =		
	{
		    sx,		0.0f,	0.0f,		0.0f,
	  	  0.0f,		- sy,	0.0f,		0.0f,
		  0.0f,		0.0f,	1.0f,		0.0f,
		 -1.0f,		1.0f,	0.0f,		1.0f		 
	};		
	glUniformMatrix4fv(matrixIdx, 1, GL_FALSE, matrix);

	mShader->UnBind();

	GLenum errorCode = glGetError();
	assert( errorCode == GL_NO_ERROR);
}

//------------------------------------------------------------------------------
TLine2DRenderer::~TLine2DRenderer()
{
	if( mVertices )
	{
		delete [] mVertices;
		mVertices = NULL;
	}	
	if( mShader )
	{
		delete mShader;
		mShader = NULL;
	}
}	


//------------------------------------------------------------------------------
void TLine2DRenderer::RenderLine(float x0, float y0, float x1, float y1)
{
    if( !isEnabled )
        return;
    
    
    if( mVertexCount > mMaxVertexCount - 2 )
    {
        printf("%d : %d\n", mVertexCount, mMaxVertexCount);
        Flush();
    }

	
	int index = mVertexCount << 1;

	mVertices[index++] = x0;
	mVertices[index++] = y0;

	mVertices[index++] = x1;
	mVertices[index++] = y1;	
	
	mVertexCount += 2;
}

//------------------------------------------------------------------------------
void TLine2DRenderer::Reset()
{
    if( mVertexCount > 0 )
        Flush();
	mVertexCount = 0;
}

//------------------------------------------------------------------------------
void TLine2DRenderer::SetColor(const TColor & color)
{
	if( color != mColor )
	{
		Flush();
		mColor = color;
	}
}

//------------------------------------------------------------------------------
void TLine2DRenderer::Flush()
{
	if( mVertexCount <= 0 )
		return;
    
    mShader->Bind();
  	glEnableVertexAttribArray(TLine2DRenderer::VertexArray);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
  
    
    glBindAttribLocation(mShader->GetProgram(), 0, "vertex");


	glVertexAttribPointer(TLine2DRenderer::VertexArray, 2, GL_FLOAT, GL_FALSE, 0, mVertices);		
	
    

	glUniform4f(mColorLocation, mColor.r, mColor.g, mColor.b, mColor.a);

	glDrawArrays(GL_LINES, 0, mVertexCount);

	mShader->UnBind();

	mVertexCount = 0;

	assert( glGetError() == GL_NO_ERROR );
}

//------------------------------------------------------------------------------
void TLine2DRenderer::SetViewportSize(float width, float height)
{	
	GLuint programId = mShader->GetProgram();
	int matrixIdx = glGetUniformLocation(programId, "MVPMatrix");
		
	mShader->Bind();

	glUniform4f(mColorLocation, mColor.r, mColor.g, mColor.b, mColor.a);
	
	const float sx = 2.0f / width;
	const float sy = 2.0f / height;	

	const float matrix[] =		
	{
		   sx,	0.0f,	0.0f,	0.0f,
	  	 0.0f,	- sy,	0.0f,	0.0f,
		 0.0f,	0.0f,	1.0f,	0.0f,
		 -1.0f,	1.0f,	0.0f,	1.0f
	};		

	glUniformMatrix4fv(matrixIdx, 1, GL_FALSE, matrix);

	mShader->UnBind();

	GLenum errorCode = glGetError();
	assert( errorCode == GL_NO_ERROR);
}

//------------------------------------------------------------------------------
void TLine2DRenderer::SetToDeviceSize()
{
    int w = Device::GetInstance()->renderWidth;
    int h = Device::GetInstance()->renderHeight;
    
	SetViewportSize((float)w,(float)h);
}

//------------------------------------------------------------------------------