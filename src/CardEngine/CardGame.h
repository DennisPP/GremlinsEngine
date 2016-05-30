//
//  CardGame.h
//  SocketBig2OSXClient
//
//  Created by dennis on 13/2/2015.
//  Copyright (c) 2015年 dennis. All rights reserved.
//

#ifndef __SocketBig2OSXClient__CardGame__
#define __SocketBig2OSXClient__CardGame__

#include <vector>

#include "CardHand.h"
#include "CardPlayer.h"
#include "CardGameHistory.h"

#include "AIPlayer.h"

namespace CardEngine
{

class TAIPlayer;
    
class TCardGame
{
public:
    TCardGame();
    
    
    
    int GetPlayerCount() const;
    void Restart();
    
    void Shuffle();
    void DistributeCardsToPlayers();
    
    const TCardPlayer& GetPlayer(int idx) const;
    
    const TCardGameHistory& GetHistory() const;
    
    int WhoHasDiamondThree() const;
    int GetCurrentTurn() const;
    
    void Dump();
    
    void SetSocket(int s);
    
    
    bool IsGameRoundFinished() const;
    bool IsAllPlayerPassed() const;
    void EmitHandAndNext(TCardPlayer& player, TCardHand& hand);
    void PassAndNext();
    
    void SocketEmitHand(int socket, int turnID , TCardHand &emitHand);
    void SocketEmitPass(int socket, int turnID);
    void SocketSetComputerPlay(int assignedID);
    void SocketStopComputerPlay(int assignedID);
    
    
    void Update(float dt);
    
    int GetWinPlayer() const;
    void SetServerAssignedTurnID(int turnID);
    int GetServerAssignedTurnID() const;
    
    void Setup(const std::vector<int>& cards);
    
    TCardPlayer* GetCurrentPlayer() const;
protected:
    
    int socket;
    TCardGameHistory history;
    
    std::vector<TCardPlayer> players;
    TCardHand hand; // hand in the sea
    int currentRound;
    int currentTurn;
    int passCount;
 
    int serverAssignedTurnID;
};
    
}

#endif /* defined(__SocketBig2OSXClient__CardGame__) */
