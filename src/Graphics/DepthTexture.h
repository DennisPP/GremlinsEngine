//
//  DepthTexture.h
//  MathsLand
//
//  Created by dennis on 3/8/14.
//
//

#ifndef _DepthTexture_
#define _DepthTexture_


//--------------------------------------------------------------------------
class TDepthTexture
{
public:
    //--------------------------------------------------------------------------
    TDepthTexture(int width, int height);
    
    void    Bind();
    void    UnBind();
    
    unsigned int  GetTexture() const;
    
    int GetWidth() const;
    int GetHeight() const;
    
private:
    void _Create();
    
    
    unsigned int   mFrameBuffer;
    unsigned int   mColorTexture;
    unsigned int   mDepthTexture;
    
    int mWidth, mHeight;
};
inline int TDepthTexture::GetWidth() const
{
    return mWidth;
}

inline int TDepthTexture::GetHeight() const
{
    return mHeight;
}

#endif /* defined(__MathsLand__DepthTexture__) */
