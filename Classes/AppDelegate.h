#ifndef __APP_DELEGATE_H__
#define __APP_DELEGATE_H__

#include "cocos2d.h"

class AppDelegate : private cocos2d::Application
{
public:
    AppDelegate();
    virtual ~AppDelegate();

    virtual void initGLContextAttrs() override;
    virtual bool applicationDidFinishLaunching() override;
    virtual void applicationDidEnterBackground() override;
    virtual void applicationWillEnterForeground() override;
};

#endif
