//
//  CardHand.cpp
//  SocketBig2OSXClient
//
//  Created by dennis on 13/2/2015.
//  Copyright (c) 2015年 dennis. All rights reserved.
//

#include "CardHand.h"


#include <cassert>
#include <vector>

namespace CardEngine
{
    

    
void Combination(int n, int r, int *arr, int sz, std::vector<std::vector<int>> & result)
{
    for (int i = r; i <= n; ++i)
    {
        // choose the first element
        arr[r-1] = i-1;
        if( r == 1 )
        {
            std::vector<int> item;
            for (int k = 0; k < sz; k ++)
            {
                //printf("%d ", arr[i]);
                item.push_back(arr[k]);
            }
            result.push_back(item);
            //printf("\n");
        }
        else if (r > 1)
        {
            Combination(i - 1, r - 1, arr, sz, result);
        }
    }
}


    
    


TCardHand::TCardHand()
{
}

void TCardHand::AddCard(const TCard& card)
{
    cards.push_back(card);
}
    
bool TCardHand::IsEmpty() const
{
    return cards.empty();
}
    
int TCardHand::GetCardCount() const
{
    return (int)cards.size();
}
    
const TCard& TCardHand::GetCard(int idx) const
{
    assert( idx>=0 && idx<cards.size());
    return cards[idx];
}

void TCardHand::SortByRank(TCardHand::TSortType type)
{
    class TSorter
    {
    public:
        TSorter(TCardHand::TSortType type)
        {
            sortType = type;
        };
        
        bool operator()(const TCard& c0, const TCard& c1)
        {
            if( sortType == TSortType::SORT_POWER_ASCENDING)
                return c0.GetRank() < c1.GetRank();
            return c0.GetRank() > c1.GetRank();
        }
        TCardHand::TSortType sortType;
        
    };
    
    std::sort(cards.begin(), cards.end(), TSorter(type));
}
    
void TCardHand::SortByPower(TCardHand::TSortType type)
{
    class TSorter
    {
    public:
        TSorter(TCardHand::TSortType type)
        {
            sortType = type;
        };
        
        bool operator()(const TCard& c0, const TCard& c1)
        {
            if( sortType == TSortType::SORT_POWER_ASCENDING)
                return c0.GetPower() < c1.GetPower();
            return c0.GetPower() > c1.GetPower();
        }
        TCardHand::TSortType sortType;
        
    };

    std::sort(cards.begin(), cards.end(), TSorter(type));
}
    
void TCardHand::CloneTo(TCardHand& hand) const
{
    hand.cards.clear();
    for(int j=0;j<cards.size();++j)
    {
        hand.cards.push_back(cards[j]);
    }
}
    
bool TCardHand::IsAllCardAreSameRank() const
{
    if( cards.size()==1 )
        return true;
    
    int rank = cards[0].GetRank();
    for(int j=1;j<(int)cards.size();++j)
    {
        if( cards[j].GetRank() != rank )
            return false;
    }
    return true;
}
    
bool TCardHand::HasValidFiveHand() const
{
    std::vector<TCardHand> validFiveHands;
    GetValid5Hand(validFiveHands);
    return ! validFiveHands.empty();
}
    
bool TCardHand::IsPair() const
{
    if( cards.size()!=2 )
        return false;
    return IsAllCardAreSameRank();
}
    
bool TCardHand::IsThreePair() const
{
    if( cards.size()!=3 )
        return false;
    return IsAllCardAreSameRank();
}

bool TCardHand::IsStrightFour() const
{
    if( cards.size()!=5 )
        return false;
    
    int slots[13];
    for(int j=0;j<13;++j)
        slots[j] = 0;
    
    for(size_t j=0;j<cards.size();++j)
        slots[cards[j].GetRank()] ++;
    
    bool has = false;
    for(int j=0;j<13;++j)
    {
        if( slots[j] == 4 )
            has = true;
    }
    return has;
}
    
bool TCardHand::IsFlush() const
{
    return IsSnake() && IsFlower();
}
    
bool TCardHand::IsSnake() const
{
    if( cards.size()!=5)
        return false;
    
    TCardHand clonedHand;
    CloneTo(clonedHand);
    
    
    class TSorter
    {
    public:
        TSorter(){};
        bool operator()(const TCard& c0, const TCard& c1)
        {
            return c0.GetRank() < c1.GetRank();
        }
    };
    
    std::sort(clonedHand.cards.begin(), clonedHand.cards.end(), TSorter());

    
    // A, K, Q, J, 10
    if( clonedHand.cards[0].GetRank()==0 &&
        clonedHand.cards[1].GetRank()==9 &&
        clonedHand.cards[2].GetRank()==10 &&
        clonedHand.cards[3].GetRank()==11 &&
        clonedHand.cards[4].GetRank()==12
           )
    {
        return true;
    }
    
    for(int j=1;j<clonedHand.cards.size();++j)
    {
        int diff = clonedHand.cards[j].GetRank() - clonedHand.cards[j-1].GetRank();
        if( abs(diff)!= 1 )
            return false;
    }
    return true;
}
    
bool TCardHand::IsFulo() const
{
    if( cards.size()!=5 )
        return false;
    int slots[13];
    for(int j=0;j<13;++j)
    {
        slots[j] = 0;
    }
    for(size_t j=0;j<cards.size();++j)
    {
        slots[cards[j].GetRank()] ++;
    }
    bool hasPair = false;
    bool hasThreePair = false;
    for(int j=0;j<13;++j)
    {
        if( slots[j] == 2 )
            hasPair = true;
        if( slots[j] == 3 )
            hasThreePair = true;
    }
    return hasPair && hasThreePair;
}
    
bool TCardHand::IsFlower() const
{
    if( cards.size()!=5)
        return false;
    
    for(int j=1;j<cards.size();++j)
    {
        if( cards[j].GetSuit() != cards[j-1].GetSuit() )
           return false;
    }
    return true;
}
    
void TCardHand::GetTwoPairList(std::vector<TCardHand>& pairs) const
{
    class TPair2Filter : public TFilter
    {
    public:
        bool IsSatisfy(TCardHand& hand) const
        {
            return hand.IsPair();
        }
    };
    GetCombination((int)cards.size(), 2 , pairs, TPair2Filter());
}
    
void TCardHand::GetThreePairList(std::vector<TCardHand>& pairs) const
{
    if( cards.size() != 3 )
        return;
    
    class TValid3Filter: public TFilter
    {
    public:
        bool IsSatisfy(TCardHand& hand) const
        {
            return hand.IsThreePair();
        }
    };
    GetCombination((int)cards.size(), 3, pairs, TValid3Filter());
}
    
void TCardHand::GetValid5Hand(std::vector<TCardHand>& hands) const
{
    if( cards.size()!=5 )
        return;
    
    class TValid5Filter: public TFilter
    {
    public:
        bool IsSatisfy(TCardHand& hand) const
        {
            hand.SortByPower();
            return hand.IsSnake() || hand.IsFlower() || hand.IsFulo() || hand.IsStrightFour();
        }
    };
    GetCombination((int)cards.size(), 5, hands, TValid5Filter());
}

bool TCardHand::IsValidFiveHand() const
{
    return IsSnake() || IsFlower() || IsFulo() || IsStrightFour();
}
    
bool TCardHand::IsValidHand() const
{
    if( cards.size()==1 )
        return true;
    if( cards.size()==2 )
        return IsPair();
    if( cards.size()==3 )
        return IsThreePair();
    if( cards.size()==5 )
        return IsValidFiveHand();
    return false;
}
    
bool TCardHand::HasBiggerHand(const TCardHand& currentHand)
{
    std::vector<TCardHand> biggerHands;
    
    GetBiggerHandList(biggerHands, currentHand);
 
    return ! biggerHands.empty();
}

void TCardHand::Shuffle()
{
    if( cards.size()<=1 )
        return;
    for(int j=0;j<100;++j)
    {
        int s0 = rand() % GetCardCount();
        int s1 = rand() % GetCardCount();
        if( s0 != s1 )
            std::swap(cards[s0], cards[s1]);
    }
}
    
bool TCardHand::IsComparable(const TCardHand& other) const
{
    if( cards.size()==0 || other.cards.size()==0 )
        return false;
    return cards.size() == other.cards.size();
}
    
bool TCardHand::operator == (const TCardHand& other) const
{
    if( cards.size() != other.cards.size())
        return false;
    
    TCardHand left, right;
    CloneTo(left);
    other.CloneTo(right);
  
    left.SortByPower();
    right.SortByPower();
    
    for(int j=0;j<left.cards.size();++j)
    {
        if( left.cards[j].GetCardID()!=right.cards[j].GetCardID() )
            return false;
    }
    return true;
}
    
bool TCardHand::IsFlowerSmallerThan(const TCardHand& other) const
{
    assert( GetCardCount()==5 && other.GetCardCount()==5 );
    
    TCardHand left, right;
    CloneTo(left);
    other.CloneTo(right);
    left.SortByPower(TSortType::SORT_POWER_DESCENDING);
    right.SortByPower(TSortType::SORT_POWER_DESCENDING);
    
    for(int j=0;j<5;++j)
    {
        const TCard& card0 = left.GetCard(j);
        const TCard& card1 = right.GetCard(j);
        if( card0.GetRank() != card1.GetRank() )
        {
            return card0.GetPower() < card1.GetPower();
        }
    }
    for(int j=0;j<5;++j)
    {
        const TCard& card0 = left.GetCard(j);
        const TCard& card1 = right.GetCard(j);
        if( card0.GetPower() != card1.GetPower() )
            return card0.GetPower() < card1.GetPower();
    }
    return false;
}
    
    
bool TCardHand::operator < (const TCardHand& other) const
{
    assert( IsComparable(other) );
    if( cards.size() == 0 )
        return false;
    if( cards.size() == 1 )
        return cards[0].GetPower() < other.cards[0].GetPower();
    else if( cards.size()==2 || cards.size()==3 )
    {
        if( cards.size()==2 )
        {
            assert( IsPair() && other.IsPair() );
        }
        if( cards.size()==3 )
        {
             assert( IsThreePair() && other.IsThreePair() );
        }
        TCardHand left, right;
        CloneTo(left);
        other.CloneTo(right);
        left.SortByPower(TSortType::SORT_POWER_DESCENDING);
        right.SortByPower(TSortType::SORT_POWER_DESCENDING);
        
        for(int j=0;j<left.cards.size();++j)
        {
            if( left.cards[j].GetPower() < right.cards[j].GetPower() )
                return true;
            if( left.cards[j].GetPower() > right.cards[j].GetPower() )
                return false;
        }
        return false;
    }
    else if( cards.size() == 5 )
    {
        TFiveHandType type0 = GetFiveHandType();
        TFiveHandType type1 = other.GetFiveHandType();
        
        if( type0 < type1 )
            return true;
        if( type0 > type1 )
            return false;
        
        if( type0==FLOWER && type1==FLOWER )
        {
            if( this->IsFlowerSmallerThan(other) )
                return true;
            if( other.IsFlowerSmallerThan(*this) )
                return false;
        }
        
        unsigned long power0 = GetFiveHandPower();
        unsigned long power1 = other.GetFiveHandPower();
        return power0 < power1;
    
    }
    return false;
}
    
bool TCardHand::IsContainsDiamondThree() const
{
    for(int c=0;c<GetCardCount();++c)
    {
        const TCard& card = cards[c];
        if( card.GetSuit()==DIAMOND && card.GetRank()==2 )
            return true;
    }
    return false;
}
    
TFiveHandType TCardHand::GetFiveHandType() const
{
    if( cards.size()!= 5 )
        return NOT_FIVE_HAND;
    if( IsFlush() )
        return FLUSH;
    if( IsStrightFour() )
        return STAIGHT4;
    if( IsFulo())
        return FULO;
    if( IsFlower())
        return FLOWER;
    if( IsSnake())
        return SNAKE;
    return NOT_FIVE_HAND;
}
    
unsigned long TCardHand::GetFiveHandPower() const
{
    assert( cards.size() == 5 );
    
    if( IsFulo() || IsStrightFour() )
    {
    
    class TSlotData
    {
    public:
        TSlotData()
        {
            count = 0;
        }
        int count;
        TCardHand hand;
        bool operator()(const TSlotData& d0, const TSlotData& d1)
        {
            return d0.count > d1.count;
        }
    };
    
    std::vector<TSlotData> slots;
    slots.resize(13);
    
    
    for(int j=0;j<(int)cards.size();++j)
    {
        int idx = cards[j].GetRank();
        slots[idx].hand.AddCard(cards[j]);
        ++slots[idx].count;
    }
    
    std::sort(slots.begin(), slots.end(), TSlotData());
    
    for(size_t j=0;j<slots.size();++j)
    {
        if( !slots[j].hand.IsEmpty() )
        {
            slots[j].hand.SortByPower(TSortType::SORT_POWER_DESCENDING);
        }
    }
    
    unsigned long factor = 52 * 52 * 52 * 52;
    unsigned long value = 0;
    int hitted = 0;
    for(size_t j=0;j<slots.size();++j)
    {
        if( slots[j].count > 0 )
        {
            for(size_t c=0;c<slots[j].hand.cards.size();++c)
            {
                value += slots[j].hand.cards[c].GetPower() * factor;
                assert( factor>=1);
                factor /= 5;
                hitted ++;
            }
        }
    }
    assert( hitted == 5 );
    
    return value;
    }
    
    
    TCardHand clonedHand;
    CloneTo(clonedHand);
    
    clonedHand.SortByPower(TSortType::SORT_POWER_ASCENDING);
    
    unsigned long factor = 1;
    unsigned long value = 0;
    for(int c=0;c<clonedHand.GetCardCount();++c)
    {
        value += clonedHand.GetCard(c).GetPower() * factor;
        factor *= 52;
    }
    return value;
}
    
bool TCardHand::IsContains(const TCardHand& hand)
{
    for(size_t i=0;i<hand.cards.size();++i)
    {
        bool found = false;
        const TCard& card0 = hand.cards[i];
        for(size_t j=0;j<cards.size();++j)
        {
            const TCard& card1 = cards[j];
            if( card0.GetCardID() == card1.GetCardID() )
            {
                found = true;
                break;
            }
        }
        if( ! found )
            return false;
    }
    return true;
}

    
void TCardHand::EmitHand(const TCardHand& hand)
{
    assert( IsContains(hand) );
    
    size_t count = cards.size();
    
    for(size_t j=0;j<hand.cards.size();++j)
    {
        const TCard& card0 = hand.cards[j];
        for(size_t i=0;i<cards.size();++i)
        {
            TCard card1 = cards[i];
            if( card0.GetCardID() == card1.GetCardID() )
            {
                cards.erase(cards.begin()+i);
                break;
            }
        }
    }
    assert( count = hand.cards.size() + cards.size() );
}

void TCardHand::Dump()
{
    printf("HAND:");
    for(size_t j=0;j<cards.size();++j)
    {
        cards[j].Dump();
        if( j<cards.size()-1)
        {
            printf(",");
        }
    }
    printf("\n");
}
    
void TCardHand::Clear()
{
    cards.clear();
}

void TCardHand::GetSelectedHand(TCardHand& hand) const
{
    hand.Clear();
    
    for(size_t c=0;c<cards.size();++c)
    {
        if( cards[c].IsSelected() )
            hand.AddCard(cards[c]);
    }
}

void TCardHand::GetBiggerHandList(std::vector<TCardHand>& hands, const TCardHand& currentHand) const
{
    if( currentHand.IsEmpty() )
        return;
    if( 1 == currentHand.GetCardCount() )
    {
        for(int j=0;j<cards.size();++j)
        {
            TCardHand r;
            r.AddCard(cards[j]);
            if( currentHand < r )
                hands.push_back(r);
        }
    }
    else if( 2 == currentHand.GetCardCount() )
    {
        std::vector<TCardHand> twoPairs;
        GetTwoPairList(twoPairs);
        for(size_t j=0;j<twoPairs.size();++j)
        {
            if( currentHand < twoPairs[j] )
                hands.push_back(twoPairs[j]);
        }
    }
    else if( 3 == currentHand.GetCardCount() )
    {
        std::vector<TCardHand> threePairs;
        GetThreePairList(threePairs);
        for(size_t j=0;j<threePairs.size();++j)
        {
            if( currentHand < threePairs[j] )
                hands.push_back(threePairs[j]);
        }
    }
    else if( 5 == currentHand.GetCardCount() )
    {
        std::vector<TCardHand> fiveHands;
        GetValid5Hand(fiveHands);
        for(size_t j=0;j<fiveHands.size();++j)
        {
            if( currentHand < fiveHands[j] )
                hands.push_back(fiveHands[j]);
        }
    }
}
    
void TCardHand::GetCombination(int n, int r, std::vector<TCardHand>& results, const TFilter& filter) const
{
    results.clear();
    int arr[r];
    std::vector<std::vector<int>> combinations;
    Combination(n, r, arr, r, combinations);
    for(size_t j=0;j<combinations.size();++j)
    {
        TCardHand h;
        assert( combinations[j].size() == r );
        for(int i=0;i<r;++i)
        {
            h.AddCard(GetCard(combinations[j][i]));
        }
        if( filter.IsSatisfy(h) )
            results.push_back(h);
    }
}
    
}
