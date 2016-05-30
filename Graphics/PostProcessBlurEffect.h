//
//  PostProcessBlurEffect.h
//
//  Created by dennis on 28/2/2015.
//  Copyright (c) 2015年 dennis. All rights reserved.
//

#ifndef __PostProcessBlurEffect__
#define __PostProcessBlurEffect__

#include "PostProcessEffect.h"

class TPostProcessBlurEffect : public TPostProcessEffect
{
public:
    typedef enum TBlurDirection
    {
        BlurHorizontal = 0,
        BlurVertical = 1,
    }   TBlurDirection;
public:
    TPostProcessBlurEffect(int width, int height, TBlurDirection direction, int count=1);
    
    void BindShaderVariables();
    
    TBlurDirection blurDirection;
    int blurCount;
};
#endif