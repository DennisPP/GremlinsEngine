#include "Shader.h"
#include "Graphics.h"

#include <cassert>

//--------------------------------------------------------------------------
TShader::TShader()
{
}

//--------------------------------------------------------------------------
TShader::~TShader()
{
	mUniformMap.clear();
}

//--------------------------------------------------------------------------
GLuint TShader::GetProgram() const
{
	return mProgram;
}

//--------------------------------------------------------------------------
GLint TShader::CreateUniform(const std::string & str)
{
	GLint uni = glGetUniformLocation( mProgram, str.c_str() );
//	    assert( uni != -1 );	
	mUniformMap[ str ] = uni;
	return uni;
}

//--------------------------------------------------------------------------
void TShader::SetUniform3f(const std::string & str, float x, float y, float z)
{
	GLint uni = mUniformMap[ str ];
	glUniform3f( uni, x, y, z );
}

//--------------------------------------------------------------------------
void TShader::SetUniform4f(const std::string & str, float x, float y, float z, float w)
{
    GLint uni = mUniformMap[ str ];
	glUniform4f( uni, x, y, z, w );        
}

//--------------------------------------------------------------------------
void TShader::SetUniform1f(const std::string & str, float x)
{
	GLint uni = mUniformMap[ str ];
	glUniform1f( uni, x );
}

//--------------------------------------------------------------------------
void TShader::SetUniform1i(const std::string & str, int x)
{
    GLint uni = mUniformMap[ str ];
	glUniform1i( uni, x );
}
    	

//--------------------------------------------------------------------------
bool TShader::_Compile(GLuint shader, const char *filename)
{
    FILE *fp ;
    char* buffer;
    const char* pFilename = filename;
    
        
    printf("Compile Shader: %s\n", filename);
    
    
    
    
    
    fp = fopen(pFilename, "rt");
	assert( fp );
    if( ! fp )
	    return false;
    
    fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
    buffer = (char*) malloc(size+1);

	memset( buffer, 0, size+1);
    	
    fseek(fp, 0, SEEK_SET);
    fread(buffer, 1, size, fp);	
	//printf("%s\n", buffer);
        
    glShaderSource(shader, 1, (const char**) &buffer, 0);
    free(buffer);

    int success;
    glCompileShader( shader);    
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    return success ? true : false;
}

//--------------------------------------------------------------------------
void TShader::Compile(const char *vs, const char *fs)
{
	GLuint vso = glCreateShader(GL_VERTEX_SHADER);
	GLuint fso = glCreateShader(GL_FRAGMENT_SHADER);

	bool c1 = _Compile( vso, vs );
	assert( c1 );
	bool c2 = _Compile( fso, fs );
	assert( c2 );
    	
	mProgram = glCreateProgram();
    	
	glAttachShader( mProgram, vso );
	glAttachShader( mProgram, fso );	
    	

//	Dump( mProgram );
//	Dump( vso );
//	Dump( fso );	
}

//--------------------------------------------------------------------------
void TShader::LinkProgram()
{
	glLinkProgram( mProgram );
	//Dump( mProgram );
}

//--------------------------------------------------------------------------
void TShader::Bind()
{
	glUseProgram( mProgram );
}

//--------------------------------------------------------------------------
void TShader::UnBind()
{
	glUseProgram( 0 );
}

//--------------------------------------------------------------------------
void TShader::Dump(GLuint handle)
{
    char* buffer = 0;
    int length = 0;

    // reset the error code
    glGetError();

    // this function accepts both TShaders AND programs, so do a type check.
    if (glIsProgram(handle))
    {
        glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length);
        if (glGetError() != GL_NO_ERROR)
            return;
        buffer = new char[length];
        glGetProgramInfoLog(handle, length, 0, buffer);
        if (glGetError() != GL_NO_ERROR)
            return;
    }
    else if (glIsShader(handle))
    {
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &length);
        if (glGetError() != GL_NO_ERROR)
            return;
        buffer = new char[length];
        glGetShaderInfoLog(handle, length, 0, buffer);
        if (glGetError() != GL_NO_ERROR)
            return;
    }

    if (buffer)
    {
       // printf("%s\n", buffer);
        delete [] buffer;
    }
}

//--------------------------------------------------------------------------