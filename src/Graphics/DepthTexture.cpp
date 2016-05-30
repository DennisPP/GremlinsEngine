//
//  DepthTexture.cpp
//  MathsLand
//
//  Created by dennis on 3/8/14.
//
//

#include "DepthTexture.h"

#include "GraphicsHeader.h"

#include <cassert>

//--------------------------------------------------------------------------
TDepthTexture::TDepthTexture(int width, int height)
{
    mWidth = width;
    mHeight = height;
    _Create();
}

//--------------------------------------------------------------------------
void TDepthTexture::_Create()
{
    //TODO: check support for depth_texture of NOT first
    
    
	glGenFramebuffers(1, &mFrameBuffer);
	glGenTextures(1, &mColorTexture);
    glGenTextures(1, &mDepthTexture);
    
    
	// color texture
	glBindTexture(GL_TEXTURE_2D, mColorTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    
    
    // depth texture
    glBindTexture(GL_TEXTURE_2D, mDepthTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    

    #ifdef GL_ES_VERSION_2_0
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mWidth, mHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);
    #else
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mWidth, mHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);
    #endif
    
    
	glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);
    
	// attachments
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorTexture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthTexture, 0);
	   
    
	GLenum status=glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		assert( ! "ERROR!!!" );
	}
    
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
	assert( glGetError() == GL_NO_ERROR );
}

//--------------------------------------------------------------------------
void TDepthTexture::Bind()
{
    glBindFramebuffer( GL_FRAMEBUFFER, mFrameBuffer );
    
	
    
    
	assert( glGetError() == GL_NO_ERROR );
}

//--------------------------------------------------------------------------
void TDepthTexture::UnBind()
{
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	assert( glGetError() == GL_NO_ERROR );
}

//--------------------------------------------------------------------------
GLuint TDepthTexture::GetTexture() const
{
    return mDepthTexture;
}

//--------------------------------------------------------------------------