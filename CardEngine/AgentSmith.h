//
//  AgentSmith.h
//  SocketBig2OSXClient
//
//  Created by dennis on 16/2/2015.
//  Copyright (c) 2015年 dennis. All rights reserved.
//

// computer card game player

#ifndef __AgentSmith__
#define __AgentSmith__

#include "AIPlayer.h"

namespace CardEngine
{

class TCardGame;
class TCardPlayer;
class TAgentSmith : public TAIPlayer
{
public:
    TAgentSmith(TCardGame* cardGame, TCardPlayer* player);
    
    virtual void Update(float dt);
    
    TCardPlayer* cardPlayer;
    TCardGame* cardGame;
};


}
#endif