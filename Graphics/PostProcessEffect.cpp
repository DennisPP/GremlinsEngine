//
//  PostProcessEffect.cpp
//  Socket2DShoot
//
//  Created by dennis on 28/2/2015.
//  Copyright (c) 2015年 dennis. All rights reserved.
//

#include "PostProcessEffect.h"

#include "Core/GremlinsFramework.h"
#include "Graphics/GraphicsHeader.h"
#include "Graphics/RenderTexture.h"
#include "Graphics/GraphicsUtils.h"

#include <cassert>

TPostProcessEffect::TPostProcessEffect(int _width, int _height)
{
    vertexShaderPath = NULL;
    fragmentShaderPath = NULL;
    
    const std::string& root = TGremlinsFramework::GetInstance()->GetAssetRoot();
    
    char vsPath[1024];
    char fsPath[1024];
#ifdef GL_ES_VERSION_2_0
        sprintf(vsPath, "%s%s",root.c_str(), "media/shaders/OldFilm.vs");
        sprintf(fsPath, "%s%s",root.c_str(), "media/shaders/OldFilm.fs");
#else
        sprintf(vsPath, "%s%s",root.c_str(), "media/shaders/ScreenQuadGL4.vs");
        sprintf(fsPath, "%s%s",root.c_str(), "media/shaders/LensGL4.fs");
#endif
    
    vertexShaderPath = strdup(vsPath);
    
    SetFragmentShaderPath(fsPath);
    
    width = _width;
    height = _height;
}



TPostProcessEffect::~TPostProcessEffect()
{
    if( vertexShaderPath )
    {
        delete [] vertexShaderPath;
    }
    if( fragmentShaderPath )
    {
        delete [] fragmentShaderPath;
    }
    if( glIsShader(vertexShader))
    {
        glDeleteShader(vertexShader);
    }
    if( glIsShader(fragmentShader))
    {
        glDeleteShader(fragmentShader);
    }
    if( glIsProgram(shaderProgram))
    {
        glDeleteProgram(shaderProgram);
    }
    if( glIsBuffer(vertexVBO) )
    {
        glDeleteBuffers(1, &vertexVBO);
    }
    if( glIsBuffer(uvVBO))
    {
        glDeleteBuffers(1, &uvVBO);
    }
    if( renderTexture )
    {
        delete renderTexture;
        renderTexture = NULL;
    }
}


void TPostProcessEffect::Build()
{
    renderTexture = new TRenderTexture(width, height);
    
    
    shaderProgram = glCreateProgram();
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    
    assert( vertexShaderPath && fragmentShaderPath );
    
    
    bool loadVSOK = TGraphicsUtils::LoadShaderSource(vertexShader, vertexShaderPath);
    assert(loadVSOK);
    
    assert( glGetError() == GL_NO_ERROR );
    
    
    bool loadFSOK = TGraphicsUtils::LoadShaderSource(fragmentShader, fragmentShaderPath);
    assert(loadFSOK);
    
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);
    
 
    assert( glGetError() == GL_NO_ERROR );
    
    
    glAttachShader(shaderProgram, vertexShader);
    
    glAttachShader(shaderProgram, fragmentShader);
    assert( glGetError() == GL_NO_ERROR);
    
    
    glBindAttribLocation(shaderProgram, 0, "vertex");
    glBindAttribLocation(shaderProgram, 1, "uv");
    
    assert( glGetError() == GL_NO_ERROR);
    
    glLinkProgram(shaderProgram);
  
#ifdef GL_ES_VERSION_2_0
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    
#else
    // use VAO and VBO in GLSL 4.1
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    assert( glGetError() == GL_NO_ERROR);
    
    float vertices[] =
    {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
    };
    float uvs[] =
    {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
    };
    
    unsigned int indices[6] =
    {
        0, 1, 2,
        0, 2, 3,
    };
    
    glGenBuffers(1, &indexVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    
    glGenBuffers (1, &vertexVBO);
    assert( vertexVBO!=0 );
    glBindBuffer (GL_ARRAY_BUFFER, vertexVBO);
    glBufferData (GL_ARRAY_BUFFER, 3 * 4 * sizeof(float), vertices, GL_STATIC_DRAW);
    assert( glGetError() == GL_NO_ERROR);
    
    glGenBuffers(1, &uvVBO);
    assert( uvVBO != 0 );
    assert( glGetError() == GL_NO_ERROR);
    glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
    assert( glGetError() == GL_NO_ERROR);
    glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(float), uvs, GL_STATIC_DRAW);
    assert( glGetError() == GL_NO_ERROR);
    
    // Bind a vertex Array
    glBindBuffer (GL_ARRAY_BUFFER, vertexVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    assert( glGetError() == GL_NO_ERROR);
    glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    assert( glGetError() == GL_NO_ERROR);
    
    
#endif
    
    
    
    diffuseLocation = glGetUniformLocation(shaderProgram, "diffuse");
    assert( diffuseLocation != -1 );
    
    
    
    
    glValidateProgram(shaderProgram);
    
    assert( glGetError() == GL_NO_ERROR);
    
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);

    
}

void TPostProcessEffect::BindShaderVariables()
{
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    assert( glGetError() ==GL_NO_ERROR );
    
    glBindAttribLocation(shaderProgram, 0, "vertex");
    glBindAttribLocation(shaderProgram, 1, "uv");
    assert( glGetError() ==GL_NO_ERROR );
    
    diffuseLocation = glGetUniformLocation(shaderProgram, "diffuse");
    assert( glGetError() ==GL_NO_ERROR );
    glUniform1i(diffuseLocation, 0);
    
    
  
}


void TPostProcessEffect::DrawScreenQuad()
{
#ifdef GL_ES_VERSION_2_0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    
    assert( glGetError() == GL_NO_ERROR);
    
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, uvs);
    assert( glGetError() == GL_NO_ERROR);
    
    
    glDrawArrays (GL_TRIANGLES, 0, vertexCount);
    
    assert( glGetError() == GL_NO_ERROR);
    
#else
    
     assert( glGetError() == GL_NO_ERROR );
    
    glBindVertexArray(vertexArrayObject);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    
    
    

    //glDrawArrays (GL_TRIANGLES, 0, 6);
    
    glDrawElements(
                   GL_TRIANGLES,      // mode
                   6,    // count
                   GL_UNSIGNED_INT,   // type
                   (void*)0           // element array buffer offset
                   );

    
    
    
#endif
    
    assert( glGetError() == GL_NO_ERROR );
    
}

void TPostProcessEffect::Process(unsigned int texture)
{
    renderTexture->Bind();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    assert( glGetError() ==GL_NO_ERROR );
    glUseProgram(shaderProgram);
   
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    assert( glGetError() ==GL_NO_ERROR );

    
    
    BindShaderVariables();
    
    DrawScreenQuad();
    
    
    assert( glGetError() ==GL_NO_ERROR );
    
    
    renderTexture->UnBind();
    glUseProgram(0);

}

unsigned int TPostProcessEffect::GetTexture() const
{
    return renderTexture->GetTexture();
}

void TPostProcessEffect::SetFragmentShaderPath(const char* path)
{
    if( fragmentShaderPath )
    {
        delete [] fragmentShaderPath;
    }
    size_t length = strlen(path);
    fragmentShaderPath = new char[length+1];
    memset(fragmentShaderPath, 0, length+1);
    strncpy(fragmentShaderPath, path, length);
}
