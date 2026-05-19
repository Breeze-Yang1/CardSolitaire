#include "CardTypes.h"

int rankValue(CardRank rank)
{
    return static_cast<int>(rank);
}

bool parseRank(const std::string& text, CardRank* outRank)
{
    if (text == "A") *outRank = CardRank::A;
    else if (text == "2") *outRank = CardRank::Two;
    else if (text == "3") *outRank = CardRank::Three;
    else if (text == "4") *outRank = CardRank::Four;
    else if (text == "5") *outRank = CardRank::Five;
    else if (text == "6") *outRank = CardRank::Six;
    else if (text == "7") *outRank = CardRank::Seven;
    else if (text == "8") *outRank = CardRank::Eight;
    else if (text == "9") *outRank = CardRank::Nine;
    else if (text == "10") *outRank = CardRank::Ten;
    else if (text == "J") *outRank = CardRank::J;
    else if (text == "Q") *outRank = CardRank::Q;
    else if (text == "K") *outRank = CardRank::K;
    else return false;
    return true;
}

bool parseSuit(const std::string& text, CardSuit* outSuit)
{
    if (text == "spade") *outSuit = CardSuit::Spade;
    else if (text == "heart") *outSuit = CardSuit::Heart;
    else if (text == "diamond") *outSuit = CardSuit::Diamond;
    else if (text == "club") *outSuit = CardSuit::Club;
    else return false;
    return true;
}

std::string rankToString(CardRank rank)
{
    switch (rank)
    {
    case CardRank::A: return "A";
    case CardRank::Two: return "2";
    case CardRank::Three: return "3";
    case CardRank::Four: return "4";
    case CardRank::Five: return "5";
    case CardRank::Six: return "6";
    case CardRank::Seven: return "7";
    case CardRank::Eight: return "8";
    case CardRank::Nine: return "9";
    case CardRank::Ten: return "10";
    case CardRank::J: return "J";
    case CardRank::Q: return "Q";
    case CardRank::K: return "K";
    }
    return "A";
}

std::string suitToString(CardSuit suit)
{
    switch (suit)
    {
    case CardSuit::Spade: return "spade";
    case CardSuit::Heart: return "heart";
    case CardSuit::Diamond: return "diamond";
    case CardSuit::Club: return "club";
    }
    return "spade";
}

bool isRedSuit(CardSuit suit)
{
    return suit == CardSuit::Heart || suit == CardSuit::Diamond;
}

std::string numberAssetPath(CardRank rank, CardSuit suit, bool big)
{
    std::string sizePrefix = big ? "big_" : "small_";
    std::string color = isRedSuit(suit) ? "red_" : "black_";
    return "number/" + sizePrefix + color + rankToString(rank) + ".png";
}

std::string suitAssetPath(CardSuit suit)
{
    return "suits/" + suitToString(suit) + ".png";
}
