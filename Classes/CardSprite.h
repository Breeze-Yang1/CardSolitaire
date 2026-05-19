#ifndef __CARD_SPRITE_H__
#define __CARD_SPRITE_H__

#include "CardTypes.h"
#include "cocos2d.h"

class CardSprite : public cocos2d::Node
{
public:
    static CardSprite* create(const CardData& card, bool faceUp);

    bool initWithCard(const CardData& card, bool faceUp);
    void setCard(const CardData& card);
    void setFaceUp(bool faceUp);
    void setSelectable(bool selectable);
    bool isFaceUp() const { return _faceUp; }
    const CardData& getCard() const { return _card; }

private:
    void rebuild();
    void buildFace();
    void buildBack();

private:
    CardData _card;
    bool _faceUp = true;
    bool _selectable = false;
};

#endif
