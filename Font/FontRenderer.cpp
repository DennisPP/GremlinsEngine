#include "FontRenderer.h"

#include <cassert>

#include <math.h>

#include "Core/GremlinsFramework.h"


#include "Graphics/Graphics.h"
#include "Graphics/TextureManager.h"
#include "Graphics/Shader.h"


//------------------------------------------------------------------------------
TFontRenderer * TFontRenderer::mInstance = 0;

//------------------------------------------------------------------------------
TFontRenderer & TFontRenderer::GetInstance()
{
	if( ! mInstance )
	{
		mInstance = new TFontRenderer();
	}
	return * mInstance;
}


void TFontRenderer::Startup()
{
	if( ! mInstance )
	{
		mInstance = new TFontRenderer();
	}
}


void TFontRenderer::Shutdown()
{
	if( mInstance )
	{
		delete mInstance;
		mInstance = NULL;
	}
}


//------------------------------------------------------------------------------
TFontRenderer::TFontRenderer()
:	mMaxVertexCount(512)
{	
	
	mColor = TColor::White;
	mTexture = 0;

	mVertexCount = 0;
	mVertices = new float[mMaxVertexCount<<1];
	mUVs =  new float[mMaxVertexCount<<1];
	
	_LoadShaders();
}


//------------------------------------------------------------------------------
TFontRenderer::~TFontRenderer()
{
	if( mVertices )
	{
		delete [] mVertices;
		mVertices = NULL;
	}
	if( mUVs )
	{
		delete [] mUVs;
		mUVs = NULL;
	}
	if( mShader )
	{
		delete mShader;
		mShader = NULL;
	}
}	


//------------------------------------------------------------------------------
float * TFontRenderer::GetMVPMatrix()
{
	const float sx = 2.0f / TGraphicsDriver::GetInstance()->GetWidth();
	const float sy = 2.0f / TGraphicsDriver::GetInstance()->GetHeight();
	static const float matrix[] =		
	{
		   sx,	0.0f,	0.0f,	0.0f,
	  	 0.0f,	 -sy,	0.0f,	0.0f,
		 0.0f,	0.0f,	1.0f,	0.0f,
		 -1.0f,	1.0f,	0.0f,	1.0f
	};		
	return (float*)matrix;
}

//------------------------------------------------------------------------------
void TFontRenderer::_LoadShaders()
{
	
	const std::string & root = TFireFramework::GetInstance()->GetAssetRoot();
	
    const std::string path = root + "media/shaders/";
    
    printf("Root : %s\n", path.c_str());
        
	char vs[512];
	sprintf(vs, "%s%s", path.c_str(), "basic2d.vs");
    
    char fs[512];
	sprintf(fs, "%s%s", path.c_str(), "basic2d.fs");
    	
	mShader = new TShader();	
	mShader->Compile(vs,fs);

	GLuint programId = mShader->GetProgram();
	glBindAttribLocation(programId, TFontRenderer::VertexArray, "vertex");
	glBindAttribLocation(programId, TFontRenderer::TexCoordArray, "uv");

	mShader->LinkProgram();
	
	
	mDiffuseLocation = glGetUniformLocation(programId,"diffuse");

	mColorLocation = glGetUniformLocation(programId,"color");

	int matrixIdx = glGetUniformLocation(programId, "MVPMatrix");
		
	mShader->Bind();

	glUniform1i(mDiffuseLocation, 0);

	glUniform4f(mColorLocation, 1.0f, 0.0f, 1.0f,1.0f);

	const float * matrix = GetMVPMatrix();
	glUniformMatrix4fv(matrixIdx, 1, GL_FALSE, matrix);

	mShader->UnBind();

	GLenum errorCode = glGetError();
	assert( errorCode == GL_NO_ERROR);
}


//------------------------------------------------------------------------------
void TFontRenderer::BindTexture(unsigned int texture)
{
	if( texture != mTexture )
	{
		Flush();
		mTexture = texture;		
	}
}


//------------------------------------------------------------------------------
//void TQuad2DRenderer::RenderQuad(TQuad2D * quad)
//{	
//	if( mVertexCount >= mMaxVertexCount - 6 )
//		Flush();
//
//	const float x0 = quad->mX;
//	const float y0 = quad->mY;
//	const float x1 = x0 + quad->mWidth;
//	const float y1 = y0 + quad->mHeight;
//
//	int index = mVertexCount << 1;
//
//	mVertices[index++] = x0;
//	mVertices[index++] = y0;
//
//	mVertices[index++] = x1;
//	mVertices[index++] = y0;
//
//	mVertices[index++] = x1;
//	mVertices[index++] = y1;
//
//	mVertices[index++] = x0;
//	mVertices[index++] = y0;
//
//	mVertices[index++] = x1;
//	mVertices[index++] = y1;
//
//	mVertices[index++] = x0;
//	mVertices[index  ] = y1;
//
//	int uvID = mVertexCount << 1;
//
//	mUVs[uvID++] = quad->mU0;
//	mUVs[uvID++] = quad->mV1;
//
//	mUVs[uvID++] = quad->mU1;
//	mUVs[uvID++] = quad->mV1;
//
//	mUVs[uvID++] = quad->mU1;
//	mUVs[uvID++] = quad->mV0;
//
//	mUVs[uvID++] = quad->mU0;
//	mUVs[uvID++] = quad->mV1;
//
//	mUVs[uvID++] = quad->mU1;
//	mUVs[uvID++] = quad->mV0;
//
//	mUVs[uvID++] = quad->mU0;
//	mUVs[uvID  ] = quad->mV0;
//
//	mVertexCount += 6;
//}

//------------------------------------------------------------------------------
void TFontRenderer::SetColor(const TColor & color)
{
	Flush();
	
	mShader->Bind();

	mColor = color;

	glUniform4f(mColorLocation,mColor.r, mColor.g, mColor.b, mColor.a);


	mShader->UnBind();
	

}

//------------------------------------------------------------------------------
void TFontRenderer::RenderQuad(TQuad2D * quad, float xc, float yc, float angle, bool flipped)
{	
    if( mVertexCount > mMaxVertexCount - 6 )
    {
        printf("%d : %d\n", mVertexCount, mMaxVertexCount);
        Flush();
    }

	const float sw = quad->mWidth / 2.0f;
	const float sh = quad->mHeight / 2.0f;
	
	int index = mVertexCount << 1;

	mVertices[index++] = -sw;
	mVertices[index++] =  sh;

	mVertices[index++] = sw;
	mVertices[index++] =  sh;

	mVertices[index++] = sw;
	mVertices[index++] = -sh;

	mVertices[index++] = -sw;
	mVertices[index++] =  sh;

	mVertices[index++] = sw;
	mVertices[index++] = -sh;

	mVertices[index++] = -sw;
	mVertices[index  ] = -sh;

    
	if( angle != 0.0f )
	{			
		int id = mVertexCount << 1;
		float vertices[12];
		
		int jj = 0;
		for(int j=0;j<6;++j)
		{
			vertices[jj]	= mVertices[id];
			vertices[jj+1]	= mVertices[id+1];
			id += 2;
			jj += 2;
		}

		id = mVertexCount << 1;
		jj = 0;
		for(int j=0;j<6;++j)
		{
			float x = vertices[jj++];
			float y = vertices[jj++];

			mVertices[id++]	= (x) * cos(angle) - (y) * sin(angle);
			mVertices[id++] = (x) * sin(angle) + (y) * cos(angle);
		}
	}

	int id = mVertexCount << 1;
	for(int j=0;j<6;++j)
	{
		mVertices[id++] += xc + sw;
		mVertices[id++] += yc + sh;		
	}

	int uvID = mVertexCount << 1;

	float u0 = flipped ? quad->mU1 : quad->mU0;
	float u1 = flipped ? quad->mU0 : quad->mU1;
	
	mUVs[uvID++] = u0;
	mUVs[uvID++] = quad->mV0;

	mUVs[uvID++] = u1;
	mUVs[uvID++] = quad->mV0;

	mUVs[uvID++] = u1;
	mUVs[uvID++] = quad->mV1;

	mUVs[uvID++] = u0;
	mUVs[uvID++] = quad->mV0;

	mUVs[uvID++] = u1;
	mUVs[uvID++] = quad->mV1;

	mUVs[uvID++] = u0;
	mUVs[uvID  ] = quad->mV1;

	mVertexCount += 6;
}

//------------------------------------------------------------------------------
void TFontRenderer::Reset()
{
	mVertexCount = 0;
}

//------------------------------------------------------------------------------
void TFontRenderer::Flush()
{
	if( mVertexCount <= 0 )
		return;
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mTexture);		
	
					

	glVertexAttribPointer(TFontRenderer::VertexArray, 2, GL_FLOAT, GL_FALSE, 0, mVertices);
	glVertexAttribPointer(TFontRenderer::TexCoordArray, 2, GL_FLOAT, GL_FALSE, 0, mUVs);
	
	mShader->Bind();		

	glDrawArrays(GL_TRIANGLES, 0, mVertexCount);

	mShader->UnBind();

	mVertexCount = 0;
}

//------------------------------------------------------------------------------