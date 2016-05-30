//
//  PostProcessSystem.cpp
//  Socket2DShoot
//
//  Created by dennis on 28/2/2015.
//  Copyright (c) 2015年 dennis. All rights reserved.
//

#include "PostProcessSystem.h"

#include "PostProcessEffect.h"


TPostProcessSystem::~TPostProcessSystem()
{
    for(size_t j=0;j<effects.size();++j)
        if( effects[j] )
            delete effects[j];
    
    effects.clear();
}

unsigned int TPostProcessSystem::Process(unsigned int texture)
{
    for(int j=0;j<effects.size();++j)
    {
        effects[j]->Process(texture);
        texture = effects[j]->GetTexture();
    }
    return texture;
}

void TPostProcessSystem::AddEffect(TPostProcessEffect* effect)
{
    effects.push_back(effect);
}
