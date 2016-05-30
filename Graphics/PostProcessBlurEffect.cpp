//
//  PostProcessBlurEffect.cpp
//  Socket2DShoot
//
//  Created by dennis on 28/2/2015.
//  Copyright (c) 2015年 dennis. All rights reserved.
//

#include "PostProcessBlurEffect.h"

#include "Core/GremlinsFramework.h"
#include "Graphics/GraphicsHeader.h"
#include "Graphics/RenderTexture.h"


TPostProcessBlurEffect::TPostProcessBlurEffect(int width, int height, TBlurDirection direction, int count)
:   TPostProcessEffect(width, height)
{
    blurDirection = direction;
    blurCount = count;
    
    const std::string& root = TGremlinsFramework::GetInstance()->GetAssetRoot();
    
    char fsPath[1024];
    
#ifdef GL_ES_VERSION_2_0
    sprintf(fsPath, "%s%s",root.c_str(), "media/shaders/Blur");
#else
    sprintf(fsPath, "%s%s",root.c_str(), "media/shaders/BlurGL4.fs");
#endif
    
    SetFragmentShaderPath(fsPath);
}

void TPostProcessBlurEffect::BindShaderVariables()
{
    
    TPostProcessEffect::BindShaderVariables();
 
    
    
    const int KernelSize = 13;
    
    float gussians[13] = {
        0.000116f,
        0.001227f,
        0.008466f,
        0.037976f,
        0.110867f,
        0.210789f,
        0.261121f,
        0.210789f,
        0.110867f,
        0.037976f,
        0.008466f,
        0.001227f,
        0.000116f,
    };

    /*
    float gussians[KernelSize] =
    {
        0.00598f,	0.060626f,	0.241843f,	0.383103f,	0.241843f,	0.060626f,	0.00598f,
    };
     */
    int texCoordOffsets = glGetUniformLocation(shaderProgram,"texCoordOffsets");
    
    float values[4*KernelSize];
    float *ptr = values;
    float texelSize = 1.0f / (float)renderTexture->GetWidth();
    
    
    for(int j=0;j<KernelSize;++j)
    {
        if( blurDirection == BlurVertical )
        {
            *ptr ++ = 0.0f;
            *ptr ++ = -0.5f*KernelSize * texelSize + texelSize * j;
            *ptr ++ = 0.0f;
            *ptr ++ = gussians[j];
        }
        else if( blurDirection == BlurHorizontal )
        {
            *ptr ++ = -0.5f*KernelSize * texelSize + texelSize * j;
            *ptr ++ = 0.0f;
            *ptr ++ = 0.0f;
            *ptr ++ = gussians[j];
        }
    }
    
    glUniform4fv(texCoordOffsets, KernelSize, values);
    

}