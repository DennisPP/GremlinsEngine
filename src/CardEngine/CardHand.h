//
//  CardHand.h
//  SocketBig2OSXClient
//
//  Created by dennis on 13/2/2015.
//  Copyright (c) 2015年 dennis. All rights reserved.
//

#ifndef __SocketBig2OSXClient__CardHand__
#define __SocketBig2OSXClient__CardHand__

#include <vector>

#include "Card.h"

namespace CardEngine
{
    
class TCardHand;
class  TFilter
{
public:
    virtual bool IsSatisfy(TCardHand& hand) const = 0;
};
    
class TCardHand
{
    
    friend class TCardGame;
    friend class TCardPlayer;
public:
    
    typedef enum TSortType
    {
        SORT_POWER_DESCENDING = 0,
        SORT_POWER_ASCENDING
    } TSortType;
    
    TCardHand();
    
    
    
    void AddCard(const TCard& card);
    int GetCardCount() const;
    const TCard& GetCard(int idx) const;
    
    bool HasValidFiveHand() const;
    
    void CloneTo(TCardHand& hand) const;
    
    bool IsComparable(const TCardHand& other) const;
    
    
    //TODO: need to modify to handle flower case
    // e.g. A(diamond), 10, 7, 9, 4
    // to   A(Spade), K, Q, J, 9
    bool operator < (const TCardHand& other) const;
    bool operator == (const TCardHand& other) const;
    
    bool IsFlowerSmallerThan(const TCardHand& other) const;
    
    
    void Shuffle();
    
    void GetTwoPairList(std::vector<TCardHand>& pairs) const;
    void GetThreePairList(std::vector<TCardHand>& pairs) const;
    
    void GetValid5Hand(std::vector<TCardHand>& hands) const;
    
    
    void GetBiggerHandList(std::vector<TCardHand>& hands, const TCardHand& currentHand) const;

    
    bool IsContainsDiamondThree() const;
    
    bool IsContains(const TCardHand& hand);
    
    void Clear();
    
    void EmitHand(const TCardHand& hand);
    void GetSelectedHand(TCardHand& hand) const;
    
    
    // sort the cards in the hand according to each card power
    // Note that this is NOT the total power of the hand
    void SortByPower(TSortType type = SORT_POWER_ASCENDING);
    void SortByRank(TSortType type = SORT_POWER_ASCENDING);
    
    bool IsAllCardAreSameRank() const;
    bool IsPair() const;
    bool IsThreePair() const;
    bool IsStrightFour() const; // got to have 5 cards, and among those 5, there are 4 cards has the same rank
    bool IsFlower() const;
    bool IsSnake() const;
    bool IsFulo() const;
    bool IsFlush() const;
    bool IsValidFiveHand() const;
    bool IsValidHand() const;
    
    
    bool HasBiggerHand(const TCardHand& currentHand);
    
    
    bool IsEmpty() const;
    
    TFiveHandType GetFiveHandType() const;
    
    void Dump();

protected:
    void GetCombination(int n, int r, std::vector<TCardHand>& results, const TFilter& filter) const;
    
    // this only used to compare 5 hand at the moment
    // since there are things like FuLo or Straight 4
    unsigned long GetFiveHandPower() const;

    
    std::vector<TCard> cards;
};
}

#endif /* defined(__SocketBig2OSXClient__CardHand__) */
