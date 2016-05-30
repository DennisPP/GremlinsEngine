#ifndef _SHADER_H_
#define _SHADER_H_

#include <string>
#include <map>

#include "Graphics/Graphics.h"

//------------------------------------------------------------------------------
class TShader
{
public:	
        
    //------------------------------------------------------------------------------
	TShader();
	~TShader();
        
    //------------------------------------------------------------------------------
    void	Compile(const char *vs, const char *fs);	
	void	LinkProgram();
    	
	GLint	CreateUniform(const std::string & str);

	void	SetUniform3f(const std::string & str, float x, float y, float z);
    void	SetUniform4f(const std::string & str, float x, float y, float z, float w);
	void	SetUniform1f(const std::string & str, float x);
    void	SetUniform1i(const std::string & str, int x);
    	
	void	Bind();
	void	UnBind();
	void	Dump(GLuint handle);	        

	GLuint GetProgram() const;


	int mDiffuseLocation;
	int mMatrixLocation;
	int mColorLocation;

    //------------------------------------------------------------------------------
private:        
    //------------------------------------------------------------------------------
	bool	_Compile(GLuint shader, const char *filename);
	
	GLuint mProgram;

	
	
    std::map<std::string, GLint> mUniformMap;        
    //------------------------------------------------------------------------------
};

#endif