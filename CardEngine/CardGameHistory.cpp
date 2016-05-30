//
//  CardGameHistory.cpp
//  SocketBig2OSXClient
//
//  Created by dennis on 15/2/2015.
//  Copyright (c) 2015年 dennis. All rights reserved.
//

#include "CardGameHistory.h"

#include <cassert>

namespace CardEngine
{
    
TPlayItem::TPlayItem()
{
    owner = -1;
}

TPlayItem::TPlayItem(const TCardHand& _hand, int _owner)
{
    hand = _hand;
    owner = _owner;
}

TCardGameHistory::TCardGameHistory()
{
}

void TCardGameHistory::Push(const TPlayItem& item)
{
    items.push_back(item);
}
    
void TCardGameHistory::Clear()
{
    items.clear();
}
    
bool TCardGameHistory::IsEmpty() const
{
    return items.empty();
}

const TPlayItem& TCardGameHistory::GetTop() const
{
    return items.back();
}
    
const TPlayItem& TCardGameHistory::Pop()
{
    assert( ! items.empty() );
    const TPlayItem& item = items.back();
    items.pop_back();
    return item;
}
    
void TCardGameHistory::GetLatestItems(std::vector<TPlayItem>& latestItems, int maxCount) const
{
    std::deque<TPlayItem>::const_reverse_iterator it = items.rbegin();
    for(int j=0;j<maxCount && it!=items.rend();++j,++it)
    {
        latestItems.push_back(*it);
    }
}


}