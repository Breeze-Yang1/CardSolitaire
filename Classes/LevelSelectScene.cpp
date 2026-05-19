#include "LevelSelectScene.h"
#include "GameScene.h"
#include <algorithm>

USING_NS_CC;

Scene* LevelSelectScene::createScene()
{
    Scene* scene = Scene::create();
    LevelSelectScene* layer = LevelSelectScene::create();
    scene->addChild(layer);
    return scene;
}

bool LevelSelectScene::init()
{
    if (!Layer::init())
    {
        return false;
    }

    _levels = LevelCatalog::loadLevels("levels/index.json");
    buildUi();

    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event*) {
        return this->handleTouch(touch->getLocation());
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

void LevelSelectScene::buildUi()
{
    _buttons.clear();
    const Vec2 visibleOrigin = Director::getInstance()->getVisibleOrigin();
    const Size visibleSize = Director::getInstance()->getVisibleSize();
    LayerColor* background = LayerColor::create(Color4B(54, 92, 126, 255),
        visibleSize.width, visibleSize.height);
    background->setPosition(visibleOrigin);
    addChild(background, -1);

    const float centerX = visibleOrigin.x + visibleSize.width * 0.5f;
    const float topY = visibleOrigin.y + visibleSize.height;
    const float bottomY = visibleOrigin.y;
    const float sideMargin = std::max(44.0f, visibleSize.width * 0.08f);
    const float titleFontSize = std::max(48.0f, std::min(72.0f, visibleSize.height * 0.042f));
    const float tipFontSize = std::max(30.0f, std::min(42.0f, visibleSize.height * 0.026f));
    const float titleY = topY - std::max(96.0f, visibleSize.height * 0.10f);

    Label* title = Label::createWithSystemFont("CARD SOLITAIRE", "Arial", titleFontSize);
    title->setPosition(Vec2(centerX, titleY));
    const float titleMaxWidth = visibleSize.width - sideMargin * 2.0f;
    if (title->getContentSize().width > titleMaxWidth)
    {
        title->setScale(titleMaxWidth / title->getContentSize().width);
    }
    addChild(title, 2);

    Label* tip = Label::createWithSystemFont("SELECT LEVEL", "Arial", tipFontSize);
    tip->setPosition(Vec2(centerX, titleY - titleFontSize * 1.35f));
    addChild(tip, 2);

    const size_t levelCount = std::max<size_t>(1, _levels.size());
    float listTop = tip->getPositionY() - std::max(96.0f, visibleSize.height * 0.055f);
    const float listBottom = bottomY + std::max(96.0f, visibleSize.height * 0.075f);
    float buttonGap = std::max(24.0f, std::min(54.0f, visibleSize.height * 0.026f));
    float buttonHeight = (listTop - listBottom - buttonGap * static_cast<float>(levelCount - 1))
        / static_cast<float>(levelCount);
    if (buttonHeight < 128.0f)
    {
        buttonGap = 18.0f;
        buttonHeight = (listTop - listBottom - buttonGap * static_cast<float>(levelCount - 1))
            / static_cast<float>(levelCount);
    }
    buttonHeight = std::max(118.0f, std::min(210.0f, buttonHeight));

    const float totalHeight = buttonHeight * static_cast<float>(levelCount)
        + buttonGap * static_cast<float>(levelCount - 1);
    const float firstCenterY = (listTop + listBottom) * 0.5f + totalHeight * 0.5f - buttonHeight * 0.5f;
    const float buttonWidth = std::max(520.0f,
        std::min(820.0f, visibleSize.width - sideMargin * 2.0f));
    const Size buttonSize(buttonWidth, buttonHeight);

    for (size_t i = 0; i < _levels.size(); ++i)
    {
        const float centerY = firstCenterY - i * (buttonHeight + buttonGap);
        Node* button = createLevelButton(_levels[i],
            Vec2(centerX - buttonWidth * 0.5f, centerY - buttonHeight * 0.5f), buttonSize);
        button->setTag(static_cast<int>(i));
        addChild(button, 3);
        _buttons.push_back(button);
    }
}

Node* LevelSelectScene::createLevelButton(const LevelInfo& info, const Vec2& position,
    const Size& buttonSize)
{
    Node* root = Node::create();
    root->setContentSize(buttonSize);
    root->setAnchorPoint(Vec2::ZERO);
    root->setPosition(position);

    DrawNode* panel = DrawNode::create();
    const Vec2 origin(0.0f, 0.0f);
    const Vec2 dest(buttonSize.width, buttonSize.height);
    panel->drawSolidRect(origin, dest, Color4F(0.95f, 0.89f, 0.66f, 1.0f));
    panel->drawRect(origin, dest, Color4F(0.42f, 0.25f, 0.12f, 1.0f));
    root->addChild(panel);

    const float nameFontSize = std::max(32.0f, std::min(48.0f, buttonSize.height * 0.24f));
    const float detailFontSize = std::max(22.0f, std::min(30.0f, buttonSize.height * 0.15f));
    const float playFontSize = std::max(30.0f, std::min(44.0f, buttonSize.height * 0.22f));
    const float leftX = std::max(42.0f, buttonSize.width * 0.08f);
    const float playX = buttonSize.width - std::max(84.0f, buttonSize.width * 0.12f);

    Label* name = Label::createWithSystemFont(info.name, "Arial", nameFontSize);
    name->setColor(Color3B(68, 46, 28));
    name->setAnchorPoint(Vec2(0.0f, 0.5f));
    name->setPosition(Vec2(leftX, buttonSize.height * 0.66f));
    root->addChild(name, 2);

    Label* stars = Label::createWithSystemFont(
        StringUtils::format("***  %ds / %ds / %ds", info.threeStarSeconds,
            info.twoStarSeconds, info.oneStarSeconds),
        "Arial", detailFontSize);
    stars->setColor(Color3B(92, 65, 37));
    stars->setAnchorPoint(Vec2(0.0f, 0.5f));
    stars->setPosition(Vec2(leftX, buttonSize.height * 0.30f));
    root->addChild(stars, 2);

    Label* play = Label::createWithSystemFont("PLAY", "Arial", playFontSize);
    play->setColor(Color3B(40, 108, 68));
    play->setPosition(Vec2(playX, buttonSize.height * 0.5f));
    root->addChild(play, 2);

    return root;
}

bool LevelSelectScene::handleTouch(const Vec2& location)
{
    for (Node* button : _buttons)
    {
        if (hitNode(button, location))
        {
            const int index = button->getTag();
            if (index >= 0 && index < static_cast<int>(_levels.size()))
            {
                Director::getInstance()->replaceScene(GameScene::createScene(_levels[index]));
            }
            return true;
        }
    }
    return true;
}

bool LevelSelectScene::hitNode(Node* node, const Vec2& location) const
{
    if (!node || !node->isVisible())
    {
        return false;
    }
    return node->getBoundingBox().containsPoint(location);
}
