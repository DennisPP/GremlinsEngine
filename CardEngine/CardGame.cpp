//
//  CardGame.cpp
//  SocketBig2OSXClient
//
//  Created by dennis on 13/2/2015.
//  Copyright (c) 2015年 dennis. All rights reserved.
//

#include "CardGame.h"

#include "CardPlayer.h"


#include <cstdlib>
#include <cassert>

#include "AgentSmith.h"
#include "AIPlayer.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

namespace CardEngine
{

TCardGame::TCardGame()
{
    socket = -1;
    
    serverAssignedTurnID = -1;
    currentRound = 0;
    currentTurn = -1;
    passCount = 0;
    
    srand((unsigned int)time(NULL));
    for(int j=0;j<52;++j)
    {
        hand.AddCard(TCard(j));
    }
    
    hand.Shuffle();
    
    players.push_back(TCardPlayer(this, "Small Gremlins"));
    players.push_back(TCardPlayer(this, "Panda"));
    players.push_back(TCardPlayer(this, "Alien"));
    players.push_back(TCardPlayer(this, "Big Gremlins"));
    
    for(int j=0;j<players.size();++j)
    {
        players[j].turnID = j;
    }

    
    players[0].SetPlayerType(TCardPlayer::ONLY_HUMAN);
    players[1].SetPlayerType(TCardPlayer::ONLY_HUMAN);
    players[2].SetPlayerType(TCardPlayer::ONLY_HUMAN);
    players[3].SetPlayerType(TCardPlayer::ONLY_HUMAN);
    
    
    DistributeCardsToPlayers();
    
    
   
    
    for(int j=0;j<GetPlayerCount();++j)
    {
        players[j].hand.SortByPower();
    }
    
    Dump();
}
    
void TCardGame::SetSocket(int s)
{
    socket = s;
}

void TCardGame::DistributeCardsToPlayers()
{
    assert( ! players.empty() );
    for(int j=0;j<hand.cards.size();++j)
    {
        int idx = j % players.size();
        const TCard& c = hand.cards[j];
        players[idx].hand.cards.push_back(c);
    }
    
    
    
    
    hand.cards.clear();
}
    
void TCardGame::Dump()
{
    // diamond, club, heart, spade
    const char* suitToDisplay[4] =
    {
        "\u2666",
        "\u2663",
        "\u2665",
        "\u2660",
    };
    const char* rankToDisplay[13] =
    {
        "A",
        "2",
        "3",
        "4",
        "5",
        "6",
        "7",
        "8",
        "9",
        "10",
        "J",
        "Q",
        "K",
    };
    
    for(size_t j=0;j<players.size();++j)
    {
        printf("%s:", players[j].GetName().c_str());
        int size = (int)players[j].hand.cards.size();
        for(int c=0;c<size;++c)
        {
            const TCard& card = players[j].hand.cards[c];
            printf("[%s%s]", suitToDisplay[card.GetSuit()], rankToDisplay[card.GetRank()]);
            if( c < size-1 )
                printf(",");
        }
       
        printf("\n");
    }
}
    
const TCardPlayer& TCardGame::GetPlayer(int idx) const
{
    assert( idx>=0 && idx<(int)players.size() );
    return players[idx];
}

int TCardGame::GetPlayerCount() const
{
    return (int)players.size();
}

void TCardGame::Restart()
{
    // clear all the cards in the game hand or players' hand
    hand.cards.clear();
    for(int j=0;j<GetPlayerCount();++j)
    {
        players[j].hand.cards.clear();
    }
    
    
    // add back the card
    for(int j=0;j<52;++j)
    {
        TCard card(j);
        hand.AddCard(TCard(j));
    }
    
    hand.Shuffle();
    
    
    DistributeCardsToPlayers();
    
    for(int j=0;j<GetPlayerCount();++j)
    {
        players[j].hand.SortByPower();
        assert( players[j].hand.GetCardCount() == 13);
    }
    
    history.Clear();
    
    
   
}

int TCardGame::GetCurrentTurn() const
{
    return currentTurn;
}
    
    
const TCardGameHistory& TCardGame::GetHistory() const
{
    return history;
}
    
int TCardGame::WhoHasDiamondThree() const
{
    for(int j=0;j<(int)players.size();++j)
    {
        for(int c=0;c<players[j].hand.GetCardCount();++c)
        {
            const TCard& card = players[j].hand.cards[c];
            if( card.GetSuit()== DIAMOND && card.GetRank() == 2 )
                return j;
        }
    }
    assert( !"ERROR:BUT IMPOSSIBLE!" );
    return -1;
}
    

    
void TCardGame::EmitHandAndNext(TCardPlayer& player, TCardHand& emitHand)
{
    TCardHand& playerHand = (TCardHand&)player.GetHand();
    
    printf("Before Emit Hand:\n");
    playerHand.Dump();
    printf("About to Emit Hand:\n");
    emitHand.Dump();
    assert( playerHand.IsContains(emitHand));
    
    
    playerHand.EmitHand(emitHand);
    emitHand.Dump();
    currentTurn = (currentTurn + 1) % players.size();
    TPlayItem item(emitHand, player.turnID);
    history.Push(item);
    
    printf("Current Player: %s\n", GetCurrentPlayer()->GetName().c_str());
    passCount = 0;
    
    
    if( GetWinPlayer() >= 0 )
    {
        printf("*****************************************\n");
        printf("%s:WIN\n", players[GetWinPlayer()].GetName().c_str());
        printf("*****************************************\n");
    }
}
    
void TCardGame::SocketEmitHand(int socket, int turnID , TCardHand &emitHand)
{
    struct EmitData
    {
        int type;
        int size;
        int turnID;
        int emitByServer;
        int cardCount;
        int cards[52];
    };
    
    EmitData data;
    int dataSize = sizeof(int) * emitHand.GetCardCount() + sizeof(int)*3;
    data.type = htonl(3);
    data.size = htonl(dataSize);
    data.turnID = htonl(turnID);
    data.emitByServer = htonl(0);
    data.cardCount = htonl(emitHand.GetCardCount());
    for(int j=0;j<emitHand.GetCardCount();++j)
    {
        data.cards[j] = htonl(emitHand.GetCard(j).GetCardID());
    }
    int totalSize = dataSize + 3*sizeof(int);
    ssize_t written = send(socket, &data, totalSize, 0);
    assert( written == totalSize );
    
    printf("Write %zd bytes to socket\n", written);
}
    
void TCardGame::SocketEmitPass(int socket, int turnID)
{
    struct PassData
    {
        int type;
        int size;
        int turnID;
        int emitByServer;
    };
    PassData data;
    data.type = htonl(4);
    data.size = htonl(2*sizeof(int));
    data.turnID = htonl(turnID);
    data.emitByServer = htonl(0);
    
    ssize_t written = send(socket, &data, sizeof(data), 0);
    assert( written == sizeof(PassData));
}

void TCardGame::SocketSetComputerPlay(int who)
{
    struct ComputerPlayData
    {
        int type;
        int size;
        int who;
    };
    ComputerPlayData data;
    data.type = htonl(5);
    data.size = htonl(4);
    data.who = htonl(who);
    
    ssize_t written = send(socket, &data, sizeof(data), 0);
    assert( written == sizeof(ComputerPlayData));
}
    
    
    void TCardGame::SocketStopComputerPlay(int who)
    {
        struct ComputerPlayData
        {
            int type;
            int size;
            int who;
        };
        ComputerPlayData data;
        data.type = htonl(6);
        data.size = htonl(4);
        data.who = htonl(who);
        
        ssize_t written = send(socket, &data, sizeof(data), 0);
        assert( written == sizeof(ComputerPlayData));
    }

    
TCardPlayer* TCardGame::GetCurrentPlayer() const
{
    if( currentTurn < 0 )
        return NULL;
    
    for(int j=0;j<(int)players.size();++j)
        if( players[j].turnID == currentTurn)
            return &(TCardPlayer&)players[j];
    return NULL;
}

bool TCardGame::IsAllPlayerPassed() const
{
    return passCount >= 3;
}
    
void TCardGame::PassAndNext()
{
    if( passCount < 3 )
    {
        assert( GetCurrentPlayer());
        printf("Current Player:%s\n", GetCurrentPlayer()->GetName().c_str());
        printf("PASS:%s!!!!!!!!\n", GetCurrentPlayer()->GetName().c_str());
   
        currentTurn = (currentTurn + 1) % players.size();
        printf("Current Player: %s\n", GetCurrentPlayer()->GetName().c_str());

        ++ passCount;
    }
}
    
void TCardGame::Update(float dt)
{
    if( IsGameRoundFinished() )
        return;
    
    for(size_t j=0;j<players.size();++j)
    {
        if( players[j].GetPlayerType() == TCardPlayer::AGENT_SMITH )
        {
            TAIPlayer* aiPlayer = players[j].GetAIPlayer();
            if( aiPlayer )
                aiPlayer->Update(dt);
        }
    }
}
    

void TCardGame::SetServerAssignedTurnID(int turnID)
{
    serverAssignedTurnID = turnID;
}

int TCardGame::GetServerAssignedTurnID() const
{
    return serverAssignedTurnID;
}

    
bool TCardGame::IsGameRoundFinished() const
{
    for(int p=0;p<GetPlayerCount();++p)
    {
        if( GetPlayer(p).GetHand().GetCardCount() == 0 )
            return true;
    }
    return false;
}
    
int TCardGame::GetWinPlayer() const
{
    for(int p=0;p<GetPlayerCount();++p)
    {
        if( GetPlayer(p).GetHand().GetCardCount() == 0 )
            return p;
    }
    return -1;

}
    
void TCardGame::Setup(const std::vector<int>& cards)
{
    // clear all the cards in the game hand or players' hand
    hand.cards.clear();
    for(int j=0;j<GetPlayerCount();++j)
    {
        players[j].hand.cards.clear();
    }    
    
    for(int j=0;j<cards.size();++j)
    {
        hand.AddCard(TCard(cards[j]));
    }
    
   
    DistributeCardsToPlayers();
    
    for(int j=0;j<GetPlayerCount();++j)
    {
        players[j].hand.SortByPower();
        assert( players[j].hand.GetCardCount() == 13);
    }
    
    history.Clear();
    
    
    int p = WhoHasDiamondThree();
    assert( p>= 0 && p<4 );
    printf("%s has Diamond 3\n", players[p].GetName().c_str());
    
    currentTurn = p;
        
    Dump();
   
}
    
    
}