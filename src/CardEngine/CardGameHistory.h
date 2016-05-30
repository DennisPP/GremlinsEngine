//
//  CardGameHistory.h
//  SocketBig2OSXClient
//
//  Created by dennis on 15/2/2015.
//  Copyright (c) 2015年 dennis. All rights reserved.
//

#ifndef __CardGameHistory__
#define __CardGameHistory__

#include <deque>

#include "CardHand.h"

namespace CardEngine
{

class TPlayItem
{
    friend class TCardGameHistory;
public:
    TPlayItem();
    TPlayItem(const TCardHand& hand, int owner = -1);
//protected:
    TCardHand hand;
    int owner;
};
    
class TCardGameHistory
{
public:
    TCardGameHistory();
    bool IsEmpty() const;
    void Push(const TPlayItem& item);
    const TPlayItem& GetTop() const;
    const TPlayItem& Pop();
    void Clear();
    void GetLatestItems(std::vector<TPlayItem>& latestItems, int maxCount = 6) const;
protected:
    std::deque<TPlayItem> items;
};

}

#endif