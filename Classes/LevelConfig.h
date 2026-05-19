#ifndef __LEVEL_CONFIG_H__
#define __LEVEL_CONFIG_H__

#include "CardTypes.h"
#include "cocos2d.h"
#include <string>
#include <vector>

struct TableauCardConfig
{
    std::string id;
    CardData card;
    cocos2d::Vec2 position;
    std::vector<std::string> coveredBy;
};

struct LevelConfig
{
    cocos2d::Size designSize;
    CardData initialWaste;
    std::vector<CardData> stock;
    std::vector<TableauCardConfig> tableau;
};

class LevelConfigLoader
{
public:
    static bool loadFromFile(const std::string& path, LevelConfig* outConfig, std::string* error);
};

#endif
