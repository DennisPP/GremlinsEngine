//
//  CardPlayer.cpp
//  SocketBig2OSXClient
//
//  Created by dennis on 13/2/2015.
//  Copyright (c) 2015年 dennis. All rights reserved.
//

#include "CardPlayer.h"
#include "AgentSmith.h"

#include "CardGame.h"

namespace CardEngine
{

TCardPlayer::TCardPlayer(TCardGame* game, const std::string& _name)
{
    cardGame = game;
    name = _name;
    turnID = -1; // NOT assigned yet
    aiPlayer= new TAgentSmith(cardGame, this);
}
    
TAIPlayer* TCardPlayer::GetAIPlayer() const
{
    return aiPlayer;
}
    
void TCardPlayer::SetPlayerType(TPlayerType type)
{
    playerType = type;
}

TCardPlayer::TPlayerType TCardPlayer::GetPlayerType() const
{
    return playerType;
}
    
bool TCardPlayer::HasCard() const
{
    return ! hand.IsEmpty();
}

const std::string& TCardPlayer::GetName() const
{
    return name;
}

const TCardHand& TCardPlayer::GetHand() const
{
    return hand;
}

}