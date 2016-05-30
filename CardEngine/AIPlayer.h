//
//  AIPlayer.h
//  SocketBig2OSXClient
//
//  Created by dennis on 16/2/2015.
//  Copyright (c) 2015年 dennis. All rights reserved.
//

#ifndef __AIPlayer__
#define __AIPlayer__

namespace CardEngine {
 
class TAIPlayer
{
public:
    virtual void Update(float dt) = 0;
};
    
    
}

#endif