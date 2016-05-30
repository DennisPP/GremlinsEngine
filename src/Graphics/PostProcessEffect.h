//  Created by dennis on 28/2/2015.
//  Copyright (c) 2015年 dennis. All rights reserved.
//

#ifndef __PostProcessEffect__
#define __PostProcessEffect__

class TRenderTexture;
class TPostProcessEffect
{
public:
    TPostProcessEffect(int width, int height);
    ~TPostProcessEffect();

    void Build();
    
    
    virtual void Process(unsigned int texture);
    
    
    unsigned int GetTexture() const;
    void SetFragmentShaderPath(const char* path);
    
protected:
    void DrawScreenQuad();
    virtual void BindShaderVariables();
    
    
    unsigned int shaderProgram;
    unsigned int fragmentShader;
    unsigned int vertexShader;
    
    TRenderTexture* renderTexture;
    
    unsigned int vertexArrayObject;
    unsigned int vertexVBO;
    unsigned int uvVBO;
    unsigned int indexVBO;
    unsigned int diffuseLocation;
    int width, height;
    
    char* vertexShaderPath;
    char* fragmentShaderPath;    
};


#endif