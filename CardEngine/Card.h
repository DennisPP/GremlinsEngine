//
//  Card.h
//  SocketBig2OSXClient
//
//  Created by dennis on 13/2/2015.
//  Copyright (c) 2015年 dennis. All rights reserved.
//

#ifndef __SocketBig2OSXClient__Card__
#define __SocketBig2OSXClient__Card__

#include <stdio.h>


namespace CardEngine
{
// DO NOT modify
typedef enum TCardSuit
{
    DIAMOND = 0,
    CLUB,
    HEART,
    SPADE
} TCardSuit;
    
typedef enum TFiveHandType
{
    NOT_FIVE_HAND = -1,
    SNAKE = 0,
    FLOWER,
    FULO,
    STAIGHT4,
    FLUSH    
} TFiveHandType;
    
    
class TRect
{
public:
    float x, y, width, height;
};
    
/* the care id arrange as follow
0  -> A(Diamond)
1  -> 2(Diamond)
2  -> 3(Diamond)
...
10 -> J(Diamond)
11 -> Q(Diamond)
12 -> K(Diamond)
13 -> A(MUI FA)
14 -> 2(MUI FA)
...
51 -> K(SPADE)
*/
    
class TCard
{
public:
    TCard(int cardID);
    TCard(int rank, int suit);
    virtual int GetRank() const;    // Ace->0 2->1 3->2,.., K->12
    virtual int GetPower() const;
    
    virtual TCardSuit GetSuit() const;
    void SetRect(float x, float y, float width, float height);
    const TRect& GetRect() const;
    int GetCardID() const;
    bool IsInside(float x, float y) const;
    bool IsSelected() const;
    void SetSelected(bool s);
    void Dump();
    
protected:
    int cardID;
    bool isSelected;
    TRect rect;
};
    
}

#endif /* defined(__SocketBig2OSXClient__Card__) */
