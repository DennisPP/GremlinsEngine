#ifndef __RenderTexture__h__
#define __RenderTexture__h__



//--------------------------------------------------------------------------
class TRenderTexture
{
public:
    //--------------------------------------------------------------------------
    TRenderTexture(int width, int height);
    ~TRenderTexture();
    void    Bind();
    void    UnBind();        

    unsigned int  GetTexture() const;
    inline int GetWidth() const;
    inline int GetHeight() const;

private:
    void _Create();


    int mWidth;
    int mHeight;
    unsigned int  mFrameBuffer;
    unsigned int  mDepthRenderBuffer;
    unsigned int  mDynamicTextureID;

    
};

inline int TRenderTexture::GetWidth() const
{
    return mWidth;
}

inline int TRenderTexture::GetHeight() const
{
    return mHeight;
}

#endif