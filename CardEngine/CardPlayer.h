//
//  CardPlayer.h
//  SocketBig2OSXClient
//
//  Created by dennis on 13/2/2015.
//  Copyright (c) 2015年 dennis. All rights reserved.
//

#ifndef __SocketBig2OSXClient__CardPlayer__
#define __SocketBig2OSXClient__CardPlayer__

#include <stdio.h>
#include <string>

#include "CardHand.h"

namespace CardEngine
{
   
class TCardGame;
class TAIPlayer;
class TCardPlayer
{
    friend class TCardGame;
    friend class TAgentSmith;
public:
    typedef enum TPlayerType
    {
        ONLY_HUMAN = 0,
        AGENT_SMITH
    } TPlayerType;
   
    
    TCardPlayer(TCardGame* cardGame, const std::string& name);
    
    const TCardHand& GetHand() const;
    
    bool HasCard() const;
    
    const std::string& GetName() const;
    
    void SetPlayerType(TPlayerType type);
    TPlayerType GetPlayerType() const;
    TAIPlayer* GetAIPlayer() const;
    
protected:
    TCardGame* cardGame;
    TPlayerType playerType;
    int turnID;
    std::string name;
    TCardHand hand;
    
    
    TAIPlayer* aiPlayer;
};

}

#endif /* defined(__SocketBig2OSXClient__CardPlayer__) */
