#include "GameScene.h"
#include "LevelSelectScene.h"
#include <algorithm>

USING_NS_CC;

namespace
{
const float kHandAreaHeight = 580.0f;
const Vec2 kStockPosition(360.0f, 290.0f);
const Vec2 kWastePosition(560.0f, 290.0f);
const Vec2 kUndoPosition(820.0f, 290.0f);

bool containsId(const std::vector<std::string>& ids, const std::string& id)
{
    return std::find(ids.begin(), ids.end(), id) != ids.end();
}

ActionInterval* createCardTravelAction(const Vec2& start, const Vec2& end, float duration)
{
    ccBezierConfig config;
    const float lift = std::max(110.0f, std::abs(end.x - start.x) * 0.28f);
    config.controlPoint_1 = start + Vec2((end.x - start.x) * 0.25f, lift);
    config.controlPoint_2 = end + Vec2((start.x - end.x) * 0.18f, lift * 0.72f);
    config.endPosition = end;
    return EaseSineOut::create(BezierTo::create(duration, config));
}
}

Scene* GameScene::createScene(const LevelInfo& levelInfo)
{
    Scene* scene = Scene::create();
    GameScene* layer = GameScene::createWithLevel(levelInfo);
    scene->addChild(layer);
    return scene;
}

GameScene* GameScene::createWithLevel(const LevelInfo& levelInfo)
{
    GameScene* ret = new (std::nothrow) GameScene();
    if (ret && ret->initWithLevel(levelInfo))
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool GameScene::init()
{
    LevelInfo defaultLevel;
    defaultLevel.id = 1;
    defaultLevel.name = "LEVEL 1";
    defaultLevel.file = "levels/level_001.json";
    defaultLevel.threeStarSeconds = 35;
    defaultLevel.twoStarSeconds = 55;
    defaultLevel.oneStarSeconds = 80;
    return initWithLevel(defaultLevel);
}

bool GameScene::initWithLevel(const LevelInfo& levelInfo)
{
    if (!Layer::init())
    {
        return false;
    }

    _levelInfo = levelInfo;
    std::string error;
    if (!LevelConfigLoader::loadFromFile(_levelInfo.file, &_levelConfig, &error))
    {
        CCLOG("Load level failed: %s", error.c_str());
        return false;
    }

    _model.reset(_levelConfig);
    buildBackground();
    buildBoard();
    syncFromModel();
    scheduleUpdate();

    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event*) {
        return this->handleTouch(touch->getLocation());
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

void GameScene::update(float delta)
{
    if (_levelCompleted)
    {
        return;
    }
    _elapsedTime += delta;
    updateTimerLabel();
}

void GameScene::buildBackground()
{
    const Size visibleSize = Director::getInstance()->getVisibleSize();

    LayerColor* mainArea = LayerColor::create(Color4B(170, 122, 70, 255),
        visibleSize.width, visibleSize.height - kHandAreaHeight);
    mainArea->setPosition(0.0f, kHandAreaHeight);
    addChild(mainArea, -2);

    LayerColor* handArea = LayerColor::create(Color4B(145, 32, 150, 255),
        visibleSize.width, kHandAreaHeight);
    handArea->setPosition(0.0f, 0.0f);
    addChild(handArea, -2);

    DrawNode* splitLine = DrawNode::create();
    splitLine->drawSegment(Vec2(0.0f, kHandAreaHeight), Vec2(visibleSize.width, kHandAreaHeight),
        4.0f, Color4F(0.95f, 0.85f, 0.45f, 1.0f));
    addChild(splitLine, -1);
}

void GameScene::buildBoard()
{
    _levelLabel = Label::createWithSystemFont(_levelInfo.name, "Arial", 42);
    _levelLabel->setAnchorPoint(Vec2(0.0f, 0.5f));
    _levelLabel->setPosition(Vec2(42.0f, 2015.0f));
    addChild(_levelLabel, 60);

    _timerLabel = Label::createWithSystemFont("TIME 00:00", "Arial", 38);
    _timerLabel->setAnchorPoint(Vec2(1.0f, 0.5f));
    _timerLabel->setPosition(Vec2(1036.0f, 2015.0f));
    addChild(_timerLabel, 60);

    _homeLabel = Label::createWithSystemFont("HOME", "Arial", 36);
    _homeLabel->setPosition(Vec2(90.0f, 198.0f));
    addChild(_homeLabel, 60);

    for (size_t i = 0; i < _levelConfig.tableau.size(); ++i)
    {
        const auto& cardConfig = _levelConfig.tableau[i];
        CardSprite* card = CardSprite::create(cardConfig.card, true);
        card->setPosition(cardConfig.position);
        addChild(card, static_cast<int>(i) + 10);
        _tableauViews[cardConfig.id] = card;
    }

    CardData stockDummy;
    _stockView = CardSprite::create(stockDummy, false);
    _stockView->setPosition(kStockPosition);
    addChild(_stockView, 50);

    _wasteView = CardSprite::create(_model.getCurrentWaste(), true);
    _wasteView->setPosition(kWastePosition);
    addChild(_wasteView, 51);

    _stockCountLabel = Label::createWithSystemFont("", "Arial", 34);
    _stockCountLabel->setPosition(kStockPosition + Vec2(0.0f, -190.0f));
    addChild(_stockCountLabel, 60);

    _undoLabel = Label::createWithSystemFont("UNDO", "Arial", 44);
    _undoLabel->setPosition(kUndoPosition);
    addChild(_undoLabel, 60);

    _messageLabel = Label::createWithSystemFont("", "Arial", 34);
    _messageLabel->setPosition(Vec2(540.0f, 520.0f));
    addChild(_messageLabel, 60);

    updateTimerLabel();
}

void GameScene::syncFromModel(const std::vector<std::string>& unlockedIds)
{
    for (const auto& state : _model.getTableau())
    {
        auto it = _tableauViews.find(state.config.id);
        if (it == _tableauViews.end())
        {
            continue;
        }

        CardSprite* view = it->second;
        const bool available = _model.isTableauCardAvailable(state.config.id);
        const bool animateUnlock = !state.removed && available && containsId(unlockedIds, state.config.id);
        view->setVisible(!state.removed);
        if (animateUnlock)
        {
            view->setFaceUp(false);
            view->setSelectable(false);
        }
        else
        {
            view->setFaceUp(available);
            view->setSelectable(available && _model.canMatch(state.config.card));
        }
    }

    _wasteView->setCard(_model.getCurrentWaste());
    _wasteView->setFaceUp(true);

    const int stockCount = static_cast<int>(_model.getStock().size());
    _stockView->setVisible(stockCount > 0);
    _stockCountLabel->setString(StringUtils::format("STOCK %d", stockCount));

    _undoLabel->setOpacity(_model.hasUndo() ? 255 : 110);
    _undoLabel->setString(StringUtils::format("UNDO %d", static_cast<int>(_model.getUndoCount())));

    if (_model.isCleared())
    {
        _messageLabel->setString("CLEAR");
        if (!_levelCompleted)
        {
            showCompletion();
        }
    }
    else
    {
        _messageLabel->setString("");
    }

    animateUnlockedCards(unlockedIds);
}

std::vector<std::string> GameScene::collectAvailableTableauIds() const
{
    std::vector<std::string> ids;
    for (const auto& state : _model.getTableau())
    {
        if (_model.isTableauCardAvailable(state.config.id))
        {
            ids.push_back(state.config.id);
        }
    }
    return ids;
}

void GameScene::animateDrawFromStock(const CardData& drawnCard)
{
    _isAnimating = true;

    const int stockCount = static_cast<int>(_model.getStock().size());
    _stockView->setVisible(stockCount > 0);
    _stockCountLabel->setString(StringUtils::format("STOCK %d", stockCount));

    CardSprite* flyingCard = CardSprite::create(drawnCard, false);
    flyingCard->setPosition(kStockPosition);
    addChild(flyingCard, 100);

    const float baseScale = flyingCard->getScaleX();
    auto flip = Sequence::create(
        DelayTime::create(0.09f),
        ScaleTo::create(0.07f, 0.05f, baseScale),
        CallFunc::create([flyingCard]() {
            flyingCard->setFaceUp(true);
        }),
        ScaleTo::create(0.08f, baseScale, baseScale),
        nullptr);

    flyingCard->runAction(Sequence::create(
        Spawn::create(
            createCardTravelAction(kStockPosition, kWastePosition, 0.32f),
            flip,
            RotateBy::create(0.32f, 8.0f),
            nullptr),
        CallFunc::create([this, flyingCard]() {
            flyingCard->removeFromParent();
            syncFromModel();
            _isAnimating = false;
        }),
        nullptr));
}

void GameScene::animateTableauMatch(const std::string& id, const CardData& card,
    const Vec2& startPosition, const std::vector<std::string>& beforeAvailable)
{
    _isAnimating = true;

    std::vector<std::string> afterAvailable = collectAvailableTableauIds();
    std::vector<std::string> unlockedIds;
    for (const auto& afterId : afterAvailable)
    {
        if (!containsId(beforeAvailable, afterId))
        {
            unlockedIds.push_back(afterId);
        }
    }

    auto it = _tableauViews.find(id);
    if (it != _tableauViews.end())
    {
        it->second->setVisible(false);
    }

    CardSprite* flyingCard = CardSprite::create(card, true);
    flyingCard->setPosition(startPosition);
    addChild(flyingCard, 100);

    const float baseScale = flyingCard->getScaleX();
    flyingCard->runAction(Sequence::create(
        Spawn::create(
            createCardTravelAction(startPosition, kWastePosition, 0.34f),
            ScaleTo::create(0.34f, baseScale * 0.88f, baseScale * 0.88f),
            RotateBy::create(0.34f, -7.0f),
            nullptr),
        CallFunc::create([this, flyingCard, unlockedIds]() {
            flyingCard->removeFromParent();
            syncFromModel(unlockedIds);
            _isAnimating = false;
        }),
        nullptr));
}

void GameScene::animateUndo(const GameStateSnapshot& beforeUndo, const GameStateSnapshot& afterUndo)
{
    const std::string restoredId = findRestoredTableauId(beforeUndo, afterUndo);
    if (!restoredId.empty())
    {
        animateUndoTableauMatch(beforeUndo, restoredId);
        return;
    }

    if (afterUndo.stock.size() == beforeUndo.stock.size() + 1)
    {
        animateUndoDraw(beforeUndo);
        return;
    }

    syncFromModel();
    _isAnimating = false;
}

void GameScene::animateUndoDraw(const GameStateSnapshot& beforeUndo)
{
    CardSprite* flyingCard = CardSprite::create(beforeUndo.currentWaste, true);
    flyingCard->setPosition(kWastePosition);
    addChild(flyingCard, 100);

    const float baseScale = flyingCard->getScaleX();
    auto flipBack = Sequence::create(
        DelayTime::create(0.10f),
        ScaleTo::create(0.07f, 0.05f, baseScale),
        CallFunc::create([flyingCard]() {
            flyingCard->setFaceUp(false);
        }),
        ScaleTo::create(0.08f, baseScale, baseScale),
        nullptr);

    _wasteView->setVisible(false);
    flyingCard->runAction(Sequence::create(
        Spawn::create(
            createCardTravelAction(kWastePosition, kStockPosition, 0.32f),
            flipBack,
            RotateBy::create(0.32f, -8.0f),
            nullptr),
        CallFunc::create([this, flyingCard]() {
            flyingCard->removeFromParent();
            _wasteView->setVisible(true);
            syncFromModel();
            _isAnimating = false;
        }),
        nullptr));
}

void GameScene::animateUndoTableauMatch(const GameStateSnapshot& beforeUndo, const std::string& restoredId)
{
    auto viewIt = _tableauViews.find(restoredId);
    if (viewIt == _tableauViews.end())
    {
        syncFromModel();
        _isAnimating = false;
        return;
    }

    CardSprite* restoredView = viewIt->second;
    const Vec2 targetPosition = restoredView->getPosition();
    CardSprite* flyingCard = CardSprite::create(beforeUndo.currentWaste, true);
    flyingCard->setPosition(kWastePosition);
    addChild(flyingCard, 100);

    _wasteView->setVisible(false);
    restoredView->setVisible(false);

    const float baseScale = flyingCard->getScaleX();
    flyingCard->runAction(Sequence::create(
        Spawn::create(
            createCardTravelAction(kWastePosition, targetPosition, 0.34f),
            ScaleTo::create(0.34f, baseScale, baseScale),
            RotateBy::create(0.34f, 7.0f),
            nullptr),
        CallFunc::create([this, flyingCard]() {
            flyingCard->removeFromParent();
            _wasteView->setVisible(true);
            syncFromModel();
            _isAnimating = false;
        }),
        nullptr));
}

void GameScene::animateUnlockedCards(const std::vector<std::string>& unlockedIds)
{
    for (const auto& id : unlockedIds)
    {
        auto it = _tableauViews.find(id);
        if (it == _tableauViews.end() || !it->second->isVisible())
        {
            continue;
        }

        CardSprite* view = it->second;
        const float baseScale = view->getScaleX();
        view->stopAllActions();
        view->setScale(baseScale, baseScale);
        view->runAction(Sequence::create(
            DelayTime::create(0.04f),
            ScaleTo::create(0.08f, 0.06f, baseScale),
            CallFunc::create([this, view, id]() {
                view->setFaceUp(true);
                const TableauCardState* state = nullptr;
                for (const auto& item : _model.getTableau())
                {
                    if (item.config.id == id)
                    {
                        state = &item;
                        break;
                    }
                }
                if (state)
                {
                    view->setSelectable(_model.canMatch(state->config.card));
                }
            }),
            ScaleTo::create(0.10f, baseScale * 1.08f, baseScale * 1.08f),
            ScaleTo::create(0.08f, baseScale, baseScale),
            nullptr));
    }
}

std::string GameScene::findRestoredTableauId(const GameStateSnapshot& beforeUndo,
    const GameStateSnapshot& afterUndo) const
{
    for (const auto& afterState : afterUndo.tableau)
    {
        if (afterState.removed)
        {
            continue;
        }

        auto it = std::find_if(beforeUndo.tableau.begin(), beforeUndo.tableau.end(),
            [&afterState](const TableauCardState& beforeState) {
                return beforeState.config.id == afterState.config.id;
            });

        if (it != beforeUndo.tableau.end() && it->removed)
        {
            return afterState.config.id;
        }
    }

    return "";
}

void GameScene::updateTimerLabel()
{
    if (!_timerLabel)
    {
        return;
    }

    const int totalSeconds = static_cast<int>(_elapsedTime);
    _timerLabel->setString(StringUtils::format("TIME %02d:%02d", totalSeconds / 60, totalSeconds % 60));
}

void GameScene::showCompletion()
{
    _levelCompleted = true;
    _finishStars = calculateStarCount(_elapsedTime, _levelInfo);

    const Size visibleSize = Director::getInstance()->getVisibleSize();
    _completionPanel = Node::create();
    _completionPanel->setContentSize(Size(760.0f, 430.0f));
    _completionPanel->setAnchorPoint(Vec2(0.5f, 0.5f));
    _completionPanel->setIgnoreAnchorPointForPosition(false);
    _completionPanel->setPosition(Vec2(visibleSize.width * 0.5f, visibleSize.height * 0.5f));
    addChild(_completionPanel, 200);

    DrawNode* panel = DrawNode::create();
    panel->drawSolidRect(Vec2(-380.0f, -215.0f), Vec2(380.0f, 215.0f),
        Color4F(0.08f, 0.10f, 0.14f, 0.92f));
    panel->drawRect(Vec2(-380.0f, -215.0f), Vec2(380.0f, 215.0f),
        Color4F(0.95f, 0.82f, 0.36f, 1.0f));
    _completionPanel->addChild(panel);

    Label* title = Label::createWithSystemFont("LEVEL CLEAR", "Arial", 58);
    title->setPosition(Vec2(0.0f, 140.0f));
    _completionPanel->addChild(title, 2);

    const int totalSeconds = static_cast<int>(_elapsedTime);
    Label* time = Label::createWithSystemFont(
        StringUtils::format("TIME %02d:%02d", totalSeconds / 60, totalSeconds % 60),
        "Arial", 42);
    time->setPosition(Vec2(0.0f, 56.0f));
    _completionPanel->addChild(time, 2);

    std::string starText;
    for (int i = 0; i < 3; ++i)
    {
        starText += i < _finishStars ? "*" : "-";
        if (i < 2)
        {
            starText += " ";
        }
    }
    Label* stars = Label::createWithSystemFont(starText, "Arial", 64);
    stars->setColor(Color3B(255, 216, 72));
    stars->setPosition(Vec2(0.0f, -28.0f));
    _completionPanel->addChild(stars, 2);

    Label* home = Label::createWithSystemFont("BACK TO LEVELS", "Arial", 36);
    home->setPosition(Vec2(0.0f, -142.0f));
    _completionPanel->addChild(home, 2);

    _completionPanel->setScale(0.72f);
    _completionPanel->runAction(EaseBackOut::create(ScaleTo::create(0.24f, 1.0f)));
}

void GameScene::returnToLevelSelect()
{
    Director::getInstance()->replaceScene(LevelSelectScene::createScene());
}

bool GameScene::handleTouch(const Vec2& location)
{
    if (_isAnimating)
    {
        return true;
    }

    if (_levelCompleted)
    {
        if (_completionPanel)
        {
            returnToLevelSelect();
        }
        return true;
    }

    if (_homeLabel && hitNode(_homeLabel, location))
    {
        returnToLevelSelect();
        return true;
    }

    if (_undoLabel && hitNode(_undoLabel, location))
    {
        const GameStateSnapshot beforeUndo = _model.createSnapshot();
        if (_model.undo())
        {
            const GameStateSnapshot afterUndo = _model.createSnapshot();
            _isAnimating = true;
            animateUndo(beforeUndo, afterUndo);
        }
        return true;
    }

    if (_stockView && _stockView->isVisible() && hitNode(_stockView, location))
    {
        CardData drawnCard = _model.getStock().back();
        if (_model.drawStock())
        {
            animateDrawFromStock(drawnCard);
        }
        return true;
    }

    for (auto it = _levelConfig.tableau.rbegin(); it != _levelConfig.tableau.rend(); ++it)
    {
        auto viewIt = _tableauViews.find(it->id);
        if (viewIt == _tableauViews.end())
        {
            continue;
        }

        CardSprite* view = viewIt->second;
        if (view->isVisible() && hitNode(view, location))
        {
            const std::vector<std::string> beforeAvailable = collectAvailableTableauIds();
            const CardData matchedCard = it->card;
            const Vec2 startPosition = view->getPosition();
            if (_model.matchTableauCard(it->id))
            {
                animateTableauMatch(it->id, matchedCard, startPosition, beforeAvailable);
            }
            return true;
        }
    }

    return true;
}

bool GameScene::hitNode(Node* node, const Vec2& location) const
{
    if (!node || !node->isVisible())
    {
        return false;
    }
    Rect rect = node->getBoundingBox();
    return rect.containsPoint(location);
}
