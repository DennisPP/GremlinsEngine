//
//  PostProcessSystem.h
//
//  Created by dennis on 28/2/2015.
//  Copyright (c) 2015年 dennis. All rights reserved.
//

#ifndef __PostProcessSystem__
#define __PostProcessSystem__

#include <vector>

class TPostProcessEffect;
class TPostProcessSystem
{

public:
    ~TPostProcessSystem();
    
    unsigned int Process(unsigned int texture);
    
    void AddEffect(TPostProcessEffect* effect);
    
    
protected:
    std::vector<TPostProcessEffect*> effects;
};

#endif