#include "GraphicsUtils.h"


#include "GraphicsHeader.h"

#include <cstdio>
#include <cassert>
#include <cstring>


unsigned int TGraphicsUtils::CreateCheckerTexture(int size, int n)
{
    int count = 4 * size * size;
    
    unsigned char * buffer = new unsigned char[count];
    
    unsigned char * ptr = buffer;
    
    
    int k = size / n;
    
    for(int j=0;j<size;++j)
    {
        for(int i=0;i<size;++i)
        {
            if( (i/k+j/k)%2 == 0 )
            {
                *ptr ++ = 255;
                *ptr ++ = 255;
                *ptr ++ = 255;
                *ptr ++ = 255;
            }
            else
            {
                *ptr ++ = 0;
                *ptr ++ = 0;
                *ptr ++ = 0;
                *ptr ++ = 64;
            }
        }
    }
    
    GLuint tex;
    glGenTextures( 1, & tex );
    glBindTexture( GL_TEXTURE_2D, tex );
    
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1 );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    glTexImage2D(	GL_TEXTURE_2D
                 ,	0
                 ,	GL_RGBA
                 ,	size
                 ,	size
                 ,	0
                 ,	GL_RGBA
                 ,	GL_UNSIGNED_BYTE
                 ,	buffer
                 );
    
    delete [] buffer;
    
    
    assert( glGetError() == GL_NO_ERROR );
    return tex;
}


bool TGraphicsUtils::LoadShaderSource(unsigned int shader, const char *filename)
{
    FILE *fp ;
    char* buffer;
    const char* pFilename = filename;
    
    
    fp = fopen(pFilename, "rt");
    if( ! fp )
    {
        printf("Failed loading:%s\n", filename);
    }
    assert( fp && filename );
    if( ! fp )
        return false;
    
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    buffer = (char*) malloc(size+1);
    
    memset( buffer, 0, size+1);
    
    fseek(fp, 0, SEEK_SET);
    fread(buffer, 1, size, fp);
    printf("%s\n", buffer);
    
    glShaderSource(shader, 1, (const char**) &buffer, 0);
    free(buffer);
    
    assert( glGetError() == GL_NO_ERROR);
    return true;
}



const char* TGraphicsUtils::StatusToString(GLenum flag)
{
    switch (flag)
    {
        case GL_COMPILE_STATUS:
            return "GL_COMPILE_STATUS";
            break;
        case GL_LINK_STATUS:
            return "GL_LINK_STATUS";
            break;
        case GL_VALIDATE_STATUS:
            return "GL_VALIDATE_STATUS";
    }
    assert(!"Status Flag NOT KNOW\n");
    return NULL;
}

bool TGraphicsUtils::CheckShaderStatus(unsigned int shader, GLenum statusFlag)
{
    GLint status;
    glGetShaderiv(shader, statusFlag, &status);
    if (status == 0)
    {
        printf("%s:failed\n", StatusToString(statusFlag));
    }
    else
    {
        printf("%s:success\n", StatusToString(statusFlag));
    }
    return status==0 ? false : true;
}

bool TGraphicsUtils::CheckProgramStatus(unsigned int program, GLenum statusFlag)
{
    GLint status;
    glGetProgramiv(program, statusFlag, &status);
    if (status == 0)
    {
        printf("%s:failed\n", StatusToString(statusFlag));
    }
    else
    {
        printf("%s:success\n", StatusToString(statusFlag));
    }
    return status==0 ? false : true;
}

void TGraphicsUtils::PrintInBlock(const char* s)
{
    for(int j=0;j<50;++j)
        printf("*");
    printf("\n");
    
    printf("%s\n", s);
    
    for(int j=0;j<50;++j)
        printf("*");
    printf("\n");
}

void TGraphicsUtils::DumpProgramInfoLog(unsigned int program)
{
    int logLength = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar*)malloc(logLength);
        glGetProgramInfoLog(program, logLength, &logLength, log);
        printf("Shader program log:\n%s\n", log);
        free(log);
    }
}

void TGraphicsUtils::DumpShaderInfoLog(unsigned int shader)
{
    int logLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar*)malloc(logLength);
        glGetShaderInfoLog(shader, logLength, &logLength, log);
        printf("Shader log:\n%s\n", log);
        free(log);
    }
}
