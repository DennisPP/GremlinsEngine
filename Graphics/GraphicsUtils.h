#ifndef _GraphicsUtils_
#define _GraphicsUtils_

#include "Graphics/Graphics.h"

class TGraphicsUtils
{
public:
    static unsigned int CreateCheckerTexture(int size, int n);
    static bool LoadShaderSource(unsigned int shader, const char *filename);
    
    static void DumpShaderInfoLog(unsigned int shader);
    static void DumpProgramInfoLog(unsigned int program);
    static bool CheckShaderStatus(unsigned int shader, GLenum statusFlag);
    static bool CheckProgramStatus(unsigned int program, GLenum statusFlag);
    static void PrintInBlock(const char* s);
    static const char* StatusToString(GLenum flag);

};

#endif
