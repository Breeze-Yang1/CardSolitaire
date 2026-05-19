#ifndef __CARD_TYPES_H__
#define __CARD_TYPES_H__

#include "cocos2d.h"
#include <string>

enum class CardSuit
{
    Spade,
    Heart,
    Diamond,
    Club
};

enum class CardRank
{
    A = 1,
    Two = 2,
    Three = 3,
    Four = 4,
    Five = 5,
    Six = 6,
    Seven = 7,
    Eight = 8,
    Nine = 9,
    Ten = 10,
    J = 11,
    Q = 12,
    K = 13
};

struct CardData
{
    CardRank rank = CardRank::A;
    CardSuit suit = CardSuit::Spade;
};

int rankValue(CardRank rank);
bool parseRank(const std::string& text, CardRank* outRank);
bool parseSuit(const std::string& text, CardSuit* outSuit);
std::string rankToString(CardRank rank);
std::string suitToString(CardSuit suit);
bool isRedSuit(CardSuit suit);
std::string numberAssetPath(CardRank rank, CardSuit suit, bool big);
std::string suitAssetPath(CardSuit suit);

#endif
