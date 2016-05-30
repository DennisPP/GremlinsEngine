#include "FontEngine.h"


#include "Graphics/TextureManager.h"
#include "Graphics/FlatRenderer.h"

#include "Core/GremlinsFramework.h"

#include <cassert>

//FreeType Headers
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#include <math.h>


#include "Graphics/Color.h"

#include <codecvt>
#include <string>
#include <locale>


namespace FontEngine
{
//------------------------------------------------------------------------------
TFontEngine * TFontEngine::mInstance = NULL;
//------------------------------------------------------------------------------

TCharData::TCharData(wchar_t _ch, int _size)
:	ch( _ch )
,	size( _size )
{
}

//------------------------------------------------------------------------------
bool TCharData::operator < ( const TCharData & g ) const
{
    if( ch == g.ch )
        return size < g.size;
    return ch < g.ch;
}

//------------------------------------------------------------------------------
bool TCharData::operator == (const TCharData & g ) const
{
    return size == g.size && this->ch == g.ch;
}

//------------------------------------------------------------------------------
TCharImage::TCharImage( TCharData & data )
:	mData(data)
{
}

//------------------------------------------------------------------------------
TCharImage::~TCharImage()
{
}

//------------------------------------------------------------------------------
inline int next_p2 ( int a )
{
    int rval=1;
    while(rval<a)
        rval<<=1;
    return rval;
}

//------------------------------------------------------------------------------
TFontEngine::TFontEngine()
{
    FT_Error      error;
    error = FT_Init_FreeType( &library );              /* initialize library */
}


//------------------------------------------------------------------------------
TFontEngine::~TFontEngine()
{
    std::map<TCharData, TCharImage *>::iterator it = mMap.begin();
    for(;it!=mMap.end();++it)
    {
        TCharImage *image = it->second;
        if( image )
        {
            FT_Done_Glyph( image->mGlyph );
            delete image;
        }
    }
    
    FT_Done_Face( face );
    
    FT_Done_FreeType(  library );
}

//------------------------------------------------------------------------------
void TFontEngine::Cache(const std::wstring& str, int size )
{
    const wchar_t *p = str.c_str();
    for(;*p;++p)
    {
        char32_t ch = *p;
        TCharData g( ch, size );
        std::map< TCharData, TCharImage *>::iterator it = mMap.find( g );
        if( it == mMap.end() )
        {
            MakeTexture( ch, size );
        }
    }
}

#if 0
void TFontEngine::Render(const char* str, int size, float x, float y, const TColor& color)
{
    if( strlen(str)>1024)
    {
        printf("WARNING:::::string to big for TFontEngine::Render");
        return;
    }
    
    wchar_t ts[1024];
    swprintf(ts, 1024, L"%s", str);
    Render(ts, size, x, y, color);
}
#endif
    
    
void TFontEngine::Render(const std::wstring& str, int size, float x, float y, const TColor & color)
{
    Render(str.c_str(), size, x, y, color);
}
    
    
    /*
void TFontEngine::Render(const std::u32string& str, int size, const Vector2& U, const Vector2& V, float x, float y, const TColor& color)
{
    Render(str.c_str(), size, U, V, x, y, color);
}
    
void TFontEngine::Render(const char32_t * str, int size, const Vector2& U, const Vector2& V, float x, float y, const TColor & color)
{
    
    GLboolean blendEnabled;
    glGetBooleanv(GL_BLEND, &blendEnabled);
    glEnable( GL_BLEND );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    TFlatRenderer* flatRenderer = TFlatRenderer::GetInstance();
    
    float x0 = x;
    float y0 = y;
    
    Vector2 ppp0(x0,y0);
    
    const char32_t* p = str;
    for(;*p;++p)
    {
        TCharData g( *p, size );
        
        std::map< TCharData, TCharImage *>::const_iterator it = mMap.find( g );
        if( it == mMap.end() )
        {
            MakeTexture(*p, size );
        }
        GLuint texture = mMap[g]->mTexture;
        
        FT_Glyph ggg = mMap[g]->mGlyph;
        
        
        FT_BBox  bbox;
        FT_Glyph_Get_CBox( ggg, FT_GLYPH_BBOX_UNSCALED , &bbox );
        
        float w = (bbox.xMax - bbox.xMin) >> 6;
        float h = (bbox.yMax - bbox.yMin) >> 6;
        
        flatRenderer->BindTexture(texture);
        
        FT_Glyph_Metrics metrics = mMap[g]->mMetrics;
        
        float ox = x;
        
        float u1 = mMap[g]->u1;
        float v1 = mMap[g]->v1;
        
        x += metrics.horiBearingX >> 6;
        // FT_Pos dy = metrics.horiBearingY >> 6;
        //float yy = y - dy;
        
        if( *p!=' ' && *p!='\n' && *p!='\t' )
        {
            //  float ddx = x - x0;
            
            Vector2 p0 = ppp0;
            Vector2 p1 = ppp0 + (w)*U;
            Vector2 p2 = ppp0 + (w)*U + h*V;
            Vector2 p3 = ppp0 + h*V;
            
            
            TFlatRenderer::TVertexData pt0(p0.x, p0.y, 0.0f, 0.0f, 0.0f);
            TFlatRenderer::TVertexData pt1(p1.x, p1.y, 0.0f,   u1, 0.0f);
            TFlatRenderer::TVertexData pt2(p2.x, p2.y, 0.0f,   u1,   v1);
            TFlatRenderer::TVertexData pt3(p3.x, p3.y, 0.0f, 0.0f,   v1);
            
            
            flatRenderer->RenderTriangle(pt0, pt1, pt2, color);
            flatRenderer->RenderTriangle(pt0, pt2, pt3, color);
        }
        x = ox + (metrics.horiAdvance >> 6);
        ppp0 += (x-ox)*U;
    }
    flatRenderer->Flush();
    if (blendEnabled) {
        glEnable(GL_BLEND);
    }
    else {
        glDisable(GL_BLEND);
    }
}
     
*/

    
    
//------------------------------------------------------------------------------
void TFontEngine::Render(const wchar_t *str, int size, float x, float y, const TColor & color)
{
    //glEnable( GL_BLEND );
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    TFlatRenderer* flatRenderer = TFlatRenderer::GetInstance();
    
    const wchar_t *p = str;
    
    float sx = x;
    for(;*p;++p)
    {
        TCharData g(*p, size );
        std::map< TCharData, TCharImage *>::const_iterator it = mMap.find( g );
        if( it == mMap.end() )
        {
            MakeTexture( *p, size );
        }
        
        GLuint texture = mMap[g]->mTexture;
        
        FT_Glyph ggg = mMap[g]->mGlyph;
        
        
        FT_BBox  bbox;
        FT_Glyph_Get_CBox( ggg, FT_GLYPH_BBOX_UNSCALED , &bbox );
        
        float w = (bbox.xMax - bbox.xMin) >> 6;
        float h = (bbox.yMax - bbox.yMin) >> 6;
        
        flatRenderer->BindTexture(texture);
        //glBindTexture( GL_TEXTURE_2D, texture );
        
        FT_Glyph_Metrics metrics = mMap[g]->mMetrics;
        
        float ox = x;
        
        float u1 = mMap[g]->u1;
        float v1 = mMap[g]->v1;
        
        x += metrics.horiBearingX >> 6;
        FT_Pos dy = metrics.horiBearingY >> 6;
        float yy = y - dy;
        
        if( *p=='\n')
        {
            x = sx;
            ox = x;
            y += h;
            continue;
        }
        else if( *p!=' ' && *p!='\t' )
        {
            TFlatRenderer::TVertexData pt0(x, yy, 0.0f, 0.0f, 0.0f);
            TFlatRenderer::TVertexData pt1(x + w, yy, 0.0f, u1, 0.0f);
            TFlatRenderer::TVertexData pt2(x + w, yy + h, 0.0f, u1, v1);
            TFlatRenderer::TVertexData pt3(x, yy + h, 0.0f, 0.0f, v1);
            
            flatRenderer->RenderTriangle(pt0, pt1, pt2, color);
            flatRenderer->RenderTriangle(pt0, pt2, pt3, color);
        }
        x = ox + (metrics.horiAdvance >> 6);
    }
    flatRenderer->Flush();
    //glDisable( GL_BLEND );
    
}
 

//------------------------------------------------------------------------------
void TFontEngine::GetTextExtent(const std::wstring& str, int size, TTextExtent & extent )
{
    extent.width  = 0.0f;
    extent.height = 0.0f;
    const wchar_t *p = str.c_str();
    for(;*p;++p)
    {
        TCharData g( *p, size );
        std::map< TCharData, TCharImage *>::const_iterator it = mMap.find( g );
        if( it == mMap.end() )
        {
            //Cache( str, size );
            assert( !"Error" );
        }
        
        FT_Glyph ggg = mMap[g]->mGlyph;
        FT_Glyph_Metrics m = mMap[g]->mMetrics;
        
        FT_BBox  bbox;
        FT_Glyph_Get_CBox( ggg, FT_GLYPH_BBOX_UNSCALED , &bbox );
        
        //float w = (bbox.xMax - bbox.xMin) >> 6;
        float h = (bbox.yMax - bbox.yMin) >> 6;
        extent.width += m.horiAdvance >> 6;
        if( h > extent.height )
            extent.height = h;
    }
}

//------------------------------------------------------------------------------
void TFontEngine::LoadTTF( const char * filename, int size )
{
    FT_Error      error;
    error = FT_New_Face( library, filename, 0, &face );		/* create face object */
    
    
    FT_Matrix     matrix;                 /* transformation matrix */
    FT_Vector     pen;                    /* untransformed origin  */
    
    
    double        angle = 0.0f;
    
    
    /* set up matrix */
    matrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L );
    matrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L );
    matrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L );
    matrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L );
    
    pen.x = 1 * 64;
    pen.y = ( 10 ) * 64;
    
    //	FT_Set_Transform( face, &matrix, &pen );
    
    
    
    
    error = FT_Set_Char_Size( face, size * 64, 0, 100, 0 );	/* set character size */
}

//------------------------------------------------------------------------------
unsigned int TFontEngine::MakeTexture( wchar_t ch, int size )
{
    
    {
        GLenum gle = glGetError();
        assert( gle == GL_NO_ERROR );
    }
    
    
    TCharData g( ch, size );
    
    
    std::map< TCharData, TCharImage *>::iterator it = mMap.find( g );
    if( it != mMap.end() )
    {
        return it->second->mTexture;
    }
    
    
    FT_Error      error;
    
    error = FT_Set_Char_Size( face, size * 64, 0, 100, 0 );	/* set character size */
    
    
    //Load the Glyph for our character.
    if( 0!=(error=FT_Load_Glyph( face, FT_Get_Char_Index( face, ch ), FT_LOAD_DEFAULT)))
    {
        assert( !"FT ERROR" );
    }
    //Move the face's glyph into a Glyph object.
    FT_Glyph glyph;
    if(FT_Get_Glyph( face->glyph, &glyph ))
    {
        assert( !"FT ERROR" );
    }
    
    {
        GLenum gle = glGetError();
        assert( gle == GL_NO_ERROR );
    }
    
    
    
    //Convert the glyph to a bitmap.
    FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal, 0, 1 );
    FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
    
    //This reference will make accessing the bitmap easier
    FT_Bitmap& bitmap = bitmap_glyph->bitmap;
    
    
    FT_GlyphSlot  slot = face->glyph;
    
    //slot->metrics;
    
    
    {
        GLenum gle = glGetError();
        
        if( gle != GL_NO_ERROR )
        {
#if 0
            const GLubyte* str = gluErrorString( gle );
            printf( "%s\n", str );
#endif
            assert(!"GL ERROR");
        }
    }
    
    
    GLuint textures[1];
    glGenTextures( 1, textures );
    
    {
        GLenum gle = glGetError();
        assert( gle == GL_NO_ERROR );
    }
    
    int width = next_p2( bitmap.width );
    int height = next_p2( bitmap.rows );
    
    
    //Allocate memory for the texture data.
    GLubyte* expanded_data = new GLubyte[ 2 * width * height];
    
    // Here We Fill In The Data For The Expanded Bitmap.
    // Notice That We Are Using A Two Channel Bitmap (One
    // Channel For Luminosity And One For Alpha Values).
    //
    // We Make All The Luminosity Values White, And Use The Freeytpe Generated Bitmap
    // To Set Up The Alpha Values. Given The Blend Function That We're Going To Use,
    // This Will Make OpenGL Render The Font Properly.
    //
    // We Use The ?: Operator To Say That The Alpha Value Which We Use
    // Will Be 0 If We Are In The Padding Zone, And Whatever
    // Is The FreeType Bitmap Otherwise.
    for(int j=0; j <height;j++)
    {
        for(int i=0; i < width; i++)
        {
            expanded_data[2*(i+j*width)]	= 255;
            expanded_data[2*(i+j*width)+1]	= (i>=bitmap.width || j>=bitmap.rows) ?
            0 : bitmap.buffer[i + bitmap.width*j];
        }
    }
    
    
    //Now we just setup some texture paramaters.
    glBindTexture( GL_TEXTURE_2D, textures[0]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    
    
    //Here we actually create the texture itself, notice
    //that we are using GL_LUMINANCE_ALPHA to indicate that
    //we are using 2 channel data.
    
    glTexImage2D(
                 GL_TEXTURE_2D
                 ,	0
                 ,	GL_LUMINANCE_ALPHA
                 ,	width
                 ,	height
                 ,	0
                 ,	GL_LUMINANCE_ALPHA
                 ,	GL_UNSIGNED_BYTE
                 ,	expanded_data 
                 );
    
    GLenum gle = glGetError();
    
    if( gle != GL_NO_ERROR )
    {
#if 0
        const GLubyte* str = gluErrorString( gle );
        printf( "%s\n", str );
#endif
        
        assert(!"GL ERROR");
    }
    
    
    //With the texture created, we don't need to expanded data anymore
    delete [] expanded_data;
    
    
    
    if( it==mMap.end())
    {
        TCharImage * gg = new TCharImage(g);
        gg->mTexture	= textures[0];
        
        gg->mSlot = slot;
        
        
        gg->mMetrics = slot->metrics;
        
        gg->u1 = (float)bitmap.width / (float)width;
        gg->v1 = (float)bitmap.rows / (float)height;
        
        gg->mGlyph = glyph;
        
        mMap[g] = gg;
    }
    else
    {
        FT_Done_Glyph( glyph );
    }
    
    return textures[0];
}

//------------------------------------------------------------------------------
TFontEngine	* TFontEngine::GetInstance()
{
    if(  ! mInstance )
    {
        mInstance = new TFontEngine();	
        
        const std::string root = TGremlinsFramework::GetInstance()->GetAssetRoot();
        
        
        mInstance->LoadTTF( (root + "media/fonts/ipag.ttf").c_str() );
        
    }
    return mInstance;
}

//------------------------------------------------------------------------------
void TFontEngine::DestroyInstance()
{
    if( mInstance )
    {
        delete mInstance;
        mInstance = NULL;		
    }
}
    
std::wstring TFontEngine::StringToWString(const std::string& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
    return myconv.from_bytes(str);
}    
    
// convert wstring to UTF-8 string
std::string TFontEngine::WStringToString(const std::wstring& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
    return myconv.to_bytes(str);
}


}
//------------------------------------------------------------------------------