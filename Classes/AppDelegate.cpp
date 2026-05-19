#include "AppDelegate.h"
#include "LevelSelectScene.h"

USING_NS_CC;

static cocos2d::Size designResolutionSize = cocos2d::Size(1080, 2080);

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate()
{
}

void AppDelegate::initGLContextAttrs()
{
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};
    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching()
{
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if (!glview)
    {
        glview = GLViewImpl::createWithRect("CardSolitaire", Rect(0, 0,
            designResolutionSize.width * 0.45f, designResolutionSize.height * 0.45f));
        director->setOpenGLView(glview);
    }

    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height,
        ResolutionPolicy::SHOW_ALL);

    director->setDisplayStats(false);
    director->setAnimationInterval(1.0f / 60.0f);

    FileUtils::getInstance()->addSearchPath("Resources");
    auto scene = LevelSelectScene::createScene();
    director->runWithScene(scene);
    return true;
}

void AppDelegate::applicationDidEnterBackground()
{
    Director::getInstance()->stopAnimation();
}

void AppDelegate::applicationWillEnterForeground()
{
    Director::getInstance()->startAnimation();
}
