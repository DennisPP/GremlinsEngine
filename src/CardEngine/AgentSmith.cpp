//
//  AgentSmith.cpp
//  SocketBig2OSXClient
//
//  Created by dennis on 16/2/2015.
//  Copyright (c) 2015年 dennis. All rights reserved.
//

#include "AgentSmith.h"

#include "CardGame.h"
#include "CardGameHistory.h"

#include <cassert>

namespace CardEngine {

TAgentSmith::TAgentSmith(TCardGame* game, TCardPlayer* player)
{
    assert( game );
    assert( player );
    cardGame = game;
    cardPlayer = player;
}
    
void TAgentSmith::Update(float dt)
{
    if( cardGame->GetWinPlayer() >= 0 )
    {
        // someone already win, no need to think
        return;
    }
    
    
    TCardPlayer* currentPlayer = cardGame->GetCurrentPlayer();
    
    if( currentPlayer != cardPlayer )
        return;
    
   if( currentPlayer->GetPlayerType() != TCardPlayer::AGENT_SMITH )
       return;
       
       
    TCardHand& playerHand = (TCardHand&)currentPlayer->GetHand();
    const TCardGameHistory& history = cardGame->GetHistory();
    if( history.IsEmpty() ) // the first hand of this round
    {
        std::vector<TCardHand> valid5Hand;
        playerHand.GetValid5Hand(valid5Hand);
        int selected = -1;
        for(int j=0;j<(int)valid5Hand.size();++j)
        {
            if( valid5Hand[j].IsContainsDiamondThree() )
            {
                selected = j;
                break;
            }
        }
        if( selected>=0 )
        {
            printf("Current Player:%s\n", currentPlayer->GetName().c_str());
            printf("Before Emit Hand:\n");
            playerHand.Dump();
            printf("About to Emit Hand:\n");
            valid5Hand[selected].Dump();
            
            cardGame->EmitHandAndNext(*currentPlayer, valid5Hand[selected]);
        }
        else
        {
            printf("Current Player:%s\n", currentPlayer->GetName().c_str());
            
            TCardHand emitHand;
            emitHand.AddCard(TCard(2,DIAMOND));
            
            assert( playerHand.IsContains(emitHand));
            
            cardGame->EmitHandAndNext(*currentPlayer, emitHand);
        }
    }
    else
    {
        TCardHand& playerHand = (TCardHand&)currentPlayer->GetHand();
        
        
        if(  cardGame->IsAllPlayerPassed() )
        {
            if( playerHand.GetCardCount() == 0 )
            {
                printf("******************************\n");
                printf("%s WIN!!!!!!\n", currentPlayer->GetName().c_str());
                printf("******************************\n");
            }
            else
            {
                if( currentPlayer->hand.HasValidFiveHand() )
                {
                    std::vector<TCardHand> validFiveHands;
                    currentPlayer->hand.GetValid5Hand(validFiveHands);
                    assert( validFiveHands.size()>0);
                    std::reverse(validFiveHands.begin(), validFiveHands.end());
                    cardGame-> EmitHandAndNext(*currentPlayer, validFiveHands[0]);
                }
                else
                {
                    std::vector<TCardHand> threePairs;
                    currentPlayer->hand.GetThreePairList(threePairs);
                    if( threePairs.size()>0)
                    {
                        std::reverse(threePairs.begin(), threePairs.end());
                         cardGame->EmitHandAndNext(*currentPlayer, threePairs[0]);
                    }
                    else
                    {
                        std::vector<TCardHand> twoPairs;
                        currentPlayer->hand.GetTwoPairList(twoPairs);
                        if( twoPairs.size()>0)
                        {
                            std::reverse(twoPairs.begin(), twoPairs.end());
                             cardGame->EmitHandAndNext(*currentPlayer, twoPairs[0]);
                        }
                        else
                        {
                            TCardHand hand;
                            hand.AddCard(playerHand.GetCard(0));
                            cardGame->EmitHandAndNext(*currentPlayer, hand);
                        }
                    }
                }
            }
        }
        else
        {
            const TPlayItem& item = history.GetTop();
            std::vector<TCardHand> biggerHands;
            playerHand.GetBiggerHandList(biggerHands, item.hand);
            if( biggerHands.empty() )
                cardGame->EmitHandAndNext(*currentPlayer, biggerHands[0]);
            else
                cardGame->PassAndNext();
        }
    }
}

}