#include "CardSprite.h"

USING_NS_CC;

namespace
{
const float kCardScale = 0.9f;
const Color3B kBackColor(35, 136, 220);
const Color3B kNormalColor(255, 255, 255);
const Color3B kSelectableColor(255, 252, 220);
}

CardSprite* CardSprite::create(const CardData& card, bool faceUp)
{
    CardSprite* ret = new (std::nothrow) CardSprite();
    if (ret && ret->initWithCard(card, faceUp))
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool CardSprite::initWithCard(const CardData& card, bool faceUp)
{
    if (!Node::init())
    {
        return false;
    }

    _card = card;
    _faceUp = faceUp;

    Sprite* base = Sprite::create("card_general.png");
    if (base)
    {
        setContentSize(base->getContentSize() * kCardScale);
    }
    setAnchorPoint(Vec2(0.5f, 0.5f));
    rebuild();
    return true;
}

void CardSprite::setCard(const CardData& card)
{
    _card = card;
    rebuild();
}

void CardSprite::setFaceUp(bool faceUp)
{
    if (_faceUp == faceUp)
    {
        return;
    }
    _faceUp = faceUp;
    rebuild();
}

void CardSprite::setSelectable(bool selectable)
{
    _selectable = selectable;
    setColor(selectable ? kSelectableColor : kNormalColor);
}

void CardSprite::rebuild()
{
    removeAllChildren();
    if (_faceUp)
    {
        buildFace();
    }
    else
    {
        buildBack();
    }
    setScale(kCardScale);
}

void CardSprite::buildFace()
{
    Sprite* base = Sprite::create("card_general.png");
    if (!base)
    {
        return;
    }

    const Size size = base->getContentSize();
    setContentSize(size);
    base->setPosition(size.width * 0.5f, size.height * 0.5f);
    addChild(base);

    Sprite* smallNumber = Sprite::create(numberAssetPath(_card.rank, _card.suit, false));
    if (smallNumber)
    {
        smallNumber->setAnchorPoint(Vec2(0.0f, 1.0f));
        smallNumber->setPosition(18.0f, size.height - 20.0f);
        addChild(smallNumber, 2);
    }

    Sprite* suit = Sprite::create(suitAssetPath(_card.suit));
    if (suit)
    {
        suit->setScale(0.55f);
        suit->setAnchorPoint(Vec2(1.0f, 1.0f));
        suit->setPosition(size.width - 20.0f, size.height - 20.0f);
        addChild(suit, 2);
    }

    Sprite* bigNumber = Sprite::create(numberAssetPath(_card.rank, _card.suit, true));
    if (bigNumber)
    {
        bigNumber->setPosition(size.width * 0.5f, size.height * 0.48f);
        addChild(bigNumber, 2);
    }
}

void CardSprite::buildBack()
{
    Sprite* back = Sprite::create("card_general.png");
    if (!back)
    {
        return;
    }

    const Size size = back->getContentSize();
    setContentSize(size);
    back->setColor(kBackColor);
    back->setPosition(size.width * 0.5f, size.height * 0.5f);
    addChild(back);

    DrawNode* lines = DrawNode::create();
    const Color4F lineColor(0.18f, 0.58f, 0.92f, 0.65f);
    for (int i = -4; i < 6; ++i)
    {
        const float x0 = i * 42.0f;
        lines->drawSegment(Vec2(x0, 16.0f), Vec2(x0 + size.height, size.height - 16.0f), 2.0f, lineColor);
    }
    addChild(lines, 2);
}
