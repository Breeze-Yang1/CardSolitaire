#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "CardSprite.h"
#include "GameModel.h"
#include "LevelCatalog.h"
#include "cocos2d.h"
#include <string>
#include <unordered_map>
#include <vector>

class GameScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene(const LevelInfo& levelInfo);
    static GameScene* createWithLevel(const LevelInfo& levelInfo);
    bool initWithLevel(const LevelInfo& levelInfo);
    virtual bool init() override;
    virtual void update(float delta) override;
    CREATE_FUNC(GameScene);

private:
    void buildBackground();
    void buildBoard();
    void syncFromModel(const std::vector<std::string>& unlockedIds = std::vector<std::string>());
    std::vector<std::string> collectAvailableTableauIds() const;
    void animateDrawFromStock(const CardData& drawnCard);
    void animateTableauMatch(const std::string& id, const CardData& card,
        const cocos2d::Vec2& startPosition, const std::vector<std::string>& beforeAvailable);
    void animateUndo(const GameStateSnapshot& beforeUndo, const GameStateSnapshot& afterUndo);
    void animateUndoDraw(const GameStateSnapshot& beforeUndo);
    void animateUndoTableauMatch(const GameStateSnapshot& beforeUndo, const std::string& restoredId);
    void animateUnlockedCards(const std::vector<std::string>& unlockedIds);
    std::string findRestoredTableauId(const GameStateSnapshot& beforeUndo,
        const GameStateSnapshot& afterUndo) const;
    void updateTimerLabel();
    void showCompletion();
    void returnToLevelSelect();
    bool handleTouch(const cocos2d::Vec2& location);
    bool hitNode(cocos2d::Node* node, const cocos2d::Vec2& location) const;

private:
    LevelInfo _levelInfo;
    GameModel _model;
    LevelConfig _levelConfig;
    std::unordered_map<std::string, CardSprite*> _tableauViews;
    CardSprite* _wasteView = nullptr;
    CardSprite* _stockView = nullptr;
    cocos2d::Label* _stockCountLabel = nullptr;
    cocos2d::Label* _undoLabel = nullptr;
    cocos2d::Label* _messageLabel = nullptr;
    cocos2d::Label* _timerLabel = nullptr;
    cocos2d::Label* _levelLabel = nullptr;
    cocos2d::Label* _homeLabel = nullptr;
    cocos2d::Node* _completionPanel = nullptr;
    float _elapsedTime = 0.0f;
    int _finishStars = 0;
    bool _levelCompleted = false;
    bool _isAnimating = false;
};

#endif
