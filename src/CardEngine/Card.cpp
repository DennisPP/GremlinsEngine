//
//  Card.cpp
//  SocketBig2OSXClient
//
//  Created by dennis on 13/2/2015.
//  Copyright (c) 2015年 dennis. All rights reserved.
//

#include "Card.h"

#include <cassert>

namespace CardEngine
{
    
TCard::TCard(int id)
{
    cardID = id;
    isSelected = false;
}
    
TCard::TCard(int rank, int suit)
{
    cardID = suit * 13 + rank;
    isSelected = false;
}
    
int TCard::GetRank() const
{
    return cardID % 13;
}

TCardSuit TCard::GetSuit() const
{
    int s = (cardID / 13);
    
    assert( s>=0 && s<4);
    return (TCardSuit)s;
}
    
int TCard::GetPower() const
{
    int p = (GetRank()+11) % 13;
    return 4 * p + GetSuit();
}
    
    
void TCard::SetRect(float x, float y, float width, float height)
{
    rect.x = x;
    rect.y = y;
    rect.width = width;
    rect.height = height;
}

const TRect& TCard::GetRect() const
{
    return rect;
}
    
bool TCard::IsInside(float x, float y) const
{
    return x>rect.x && y>rect.y && x<rect.x+rect.width && y<rect.y+rect.height;
}
    
int TCard::GetCardID() const
{
    return cardID;
}
   
bool TCard::IsSelected() const
{
    return isSelected;
}
void TCard::SetSelected(bool s)
{
    isSelected = s;
}

void TCard::Dump()
{
    // diamond, club, heart, spade
    const char* suitToDisplay[4] = { "\u2666", "\u2663", "\u2665", "\u2660", };
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
    printf("[%s%s]", suitToDisplay[GetSuit()], rankToDisplay[GetRank()]);
}
    
}