#include "AppDelegate.h"
#include "cocos2d.h"
#include <tchar.h>
#include <windows.h>

USING_NS_CC;

int WINAPI _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR lpCmdLine,
                     int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    AppDelegate app;
    return Application::getInstance()->run();
}
