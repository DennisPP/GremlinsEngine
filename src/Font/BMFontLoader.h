#ifndef __FireEngine__Font__BMFontLoader__h__
#define __FireEngine__Font__BMFontLoader__h__

class TBMFont;

//------------------------------------------------------------------------------
class TBMFontLoader
{
public:

	static void Load(const char * filename, TBMFont * font);
};
//------------------------------------------------------------------------------

#endif