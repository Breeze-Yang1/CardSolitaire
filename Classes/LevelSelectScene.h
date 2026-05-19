#ifndef __LEVEL_SELECT_SCENE_H__
#define __LEVEL_SELECT_SCENE_H__

#include "LevelCatalog.h"
#include "cocos2d.h"
#include <vector>

class LevelSelectScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init() override;
    CREATE_FUNC(LevelSelectScene);

private:
    void buildUi();
    cocos2d::Node* createLevelButton(const LevelInfo& info, const cocos2d::Vec2& position,
        const cocos2d::Size& buttonSize);
    bool handleTouch(const cocos2d::Vec2& location);
    bool hitNode(cocos2d::Node* node, const cocos2d::Vec2& location) const;

private:
    std::vector<LevelInfo> _levels;
    std::vector<cocos2d::Node*> _buttons;
};

#endif
