#ifndef __FontEngine__h__
#define __FontEngine__h__

#include <string>
#include <map>

#include "ft2build.h"
#include FT_FREETYPE_H
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#include "Graphics/Color.h"
#include "Math3D/Vector2.h"



namespace FontEngine
{

    
class TTextExtent
{
public:
    float width;
    float height;
};

class TCharData
{
public:
    TCharData(wchar_t ch, int size );
    
    bool	operator < ( const TCharData & g ) const;
    bool	operator == (const TCharData & g ) const;
    wchar_t	ch;
    int		size;
};


class TCharImage
{
public:
    TCharImage( TCharData & data );
    ~TCharImage();
    
    float				u1;
    float				v1;
    TCharData			mData;
    unsigned int		mTexture;
    FT_Glyph			mGlyph;
    FT_GlyphSlot		mSlot;
    FT_Glyph_Metrics	mMetrics;
};


//------------------------------------------------------------------------------
class TFontEngine
{
public:
    
    ~TFontEngine();
    
    void Render(const std::wstring& str, int size, float x, float y, const TColor & color = TColor(0.0f, 0.0f, 0.0f, 1.0f));
    
    
    void Render(const wchar_t * str, int size, float x, float y, const TColor & color = TColor(0.0f, 0.0f, 0.0f, 1.0f));
    
    /*
    void Render(const std::u32string& str, int size, const Vector2& U, const Vector2& V, float x, float y, const TColor& color);
    void Render(const char32_t * str, int size, const Vector2& U, const Vector2& V, float x, float y, const TColor& color);
     */
    
    void	GetTextExtent( const std::wstring& str, int size, TTextExtent & extent );
    void	Cache( const std::wstring& str, int size=36);
    
    static TFontEngine	* GetInstance();
    static void	DestroyInstance();
    
    static std::wstring StringToWString(const std::string& s);
    static std::string WStringToString(const std::wstring& ws);
    
private:
    TFontEngine();
    void	LoadTTF( const char * filename, int size=36);
    unsigned int MakeTexture(wchar_t ch, int size=36);
    
    static TFontEngine	* mInstance;
    std::map<TCharData , TCharImage *>	mMap;
    
    FT_Library    library;
    FT_Face       face;
};
}
#endif