#include "RenderTexture.h"

#include "GraphicsHeader.h"


#include <stdio.h>


#include <cassert>

//--------------------------------------------------------------------------
TRenderTexture::TRenderTexture(int width, int height)
{
    mWidth = width;
    mHeight = height;
    _Create();
}


TRenderTexture::~TRenderTexture()
{
    if( glIsFramebuffer(mFrameBuffer))
    {
        glDeleteFramebuffers(1, &mFrameBuffer);
    }
}


//--------------------------------------------------------------------------    
void TRenderTexture::_Create()
{
    glGenFramebuffers( 1, & mFrameBuffer );
	glGenRenderbuffers( 1, & mDepthRenderBuffer );

	assert( glGetError() == GL_NO_ERROR );

	glBindRenderbuffer( GL_RENDERBUFFER, mDepthRenderBuffer );

	assert( glGetError() == GL_NO_ERROR );

    
#if GL_ES_VERSION_2_0
    glRenderbufferStorage(
                          GL_RENDERBUFFER
                          ,	GL_DEPTH_COMPONENT16
                          ,   mWidth
                          ,   mHeight
                          );
#else
	glRenderbufferStorage(
                                GL_RENDERBUFFER
                            ,	GL_DEPTH_COMPONENT24
							,   mWidth
                            ,   mHeight
                            );
#endif

	assert( glGetError() == GL_NO_ERROR );
    	
	GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );

	switch( status )
	{
		case GL_FRAMEBUFFER_COMPLETE:
            printf("Render Texture Create OK:\n");
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
            printf("Render Texture Create Failed:\n");
            
            break;
		default:
            {}
			    
	}	

	glGenTextures( 1, & mDynamicTextureID );

	glBindTexture( GL_TEXTURE_2D, mDynamicTextureID );
    
    
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    // clamp to edge
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D( 
                    GL_TEXTURE_2D
                ,   0
                ,   GL_RGBA
                ,   mWidth
                ,   mHeight
                ,   0
                ,   GL_RGBA
                ,   GL_UNSIGNED_BYTE
                ,   0 
                );

	
    
   

    glBindTexture( GL_TEXTURE_2D, 0 );

	assert( glGetError() == GL_NO_ERROR );
}

//--------------------------------------------------------------------------
void TRenderTexture::Bind()
{
    glBindFramebuffer( GL_FRAMEBUFFER, mFrameBuffer );
	glFramebufferTexture2D( 
                                    GL_FRAMEBUFFER
                                ,   GL_COLOR_ATTACHMENT0
                                ,   GL_TEXTURE_2D
                                ,   mDynamicTextureID
                                ,   0 
                                );


	assert( glGetError() == GL_NO_ERROR );
    
    
   
    
    

	glFramebufferRenderbuffer( 
                                    GL_FRAMEBUFFER
                                ,   GL_DEPTH_ATTACHMENT
                                ,   GL_RENDERBUFFER
                                ,   mDepthRenderBuffer 
                                );        
	
	GLenum completed = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	assert( completed == GL_FRAMEBUFFER_COMPLETE);


	assert( glGetError() == GL_NO_ERROR );
}

//--------------------------------------------------------------------------
void TRenderTexture::UnBind()        
{
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );		   
	assert( glGetError() == GL_NO_ERROR );
}  

//--------------------------------------------------------------------------
GLuint TRenderTexture::GetTexture() const
{
    return mDynamicTextureID;
}

//--------------------------------------------------------------------------