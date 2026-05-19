#include "LevelCatalog.h"
#include "cocos2d.h"
#include "json/document.h"

USING_NS_CC;

namespace
{
LevelInfo defaultLevel(int id, const std::string& file, int three, int two, int one)
{
    LevelInfo info;
    info.id = id;
    info.name = StringUtils::format("LEVEL %d", id);
    info.file = file;
    info.threeStarSeconds = three;
    info.twoStarSeconds = two;
    info.oneStarSeconds = one;
    return info;
}
}

std::vector<LevelInfo> LevelCatalog::loadLevels(const std::string& path)
{
    std::vector<LevelInfo> levels;
    const std::string content = FileUtils::getInstance()->getStringFromFile(path);
    if (content.empty())
    {
        levels.push_back(defaultLevel(1, "levels/level_001.json", 35, 55, 80));
        levels.push_back(defaultLevel(2, "levels/level_002.json", 55, 85, 120));
        levels.push_back(defaultLevel(3, "levels/level_003.json", 80, 120, 170));
        return levels;
    }

    rapidjson::Document doc;
    doc.Parse<0>(content.c_str());
    if (doc.HasParseError() || !doc.IsObject() || !doc.HasMember("levels") || !doc["levels"].IsArray())
    {
        levels.push_back(defaultLevel(1, "levels/level_001.json", 35, 55, 80));
        levels.push_back(defaultLevel(2, "levels/level_002.json", 55, 85, 120));
        levels.push_back(defaultLevel(3, "levels/level_003.json", 80, 120, 170));
        return levels;
    }

    const rapidjson::Value& levelArray = doc["levels"];
    for (rapidjson::SizeType i = 0; i < levelArray.Size(); ++i)
    {
        const rapidjson::Value& item = levelArray[i];
        if (!item.IsObject() || !item.HasMember("file") || !item["file"].IsString())
        {
            continue;
        }

        LevelInfo info;
        info.id = item.HasMember("id") ? item["id"].GetInt() : static_cast<int>(i + 1);
        info.name = item.HasMember("name") && item["name"].IsString()
            ? item["name"].GetString()
            : StringUtils::format("LEVEL %d", info.id);
        info.file = item["file"].GetString();

        if (item.HasMember("stars") && item["stars"].IsObject())
        {
            const rapidjson::Value& stars = item["stars"];
            if (stars.HasMember("three") && stars["three"].IsInt()) info.threeStarSeconds = stars["three"].GetInt();
            if (stars.HasMember("two") && stars["two"].IsInt()) info.twoStarSeconds = stars["two"].GetInt();
            if (stars.HasMember("one") && stars["one"].IsInt()) info.oneStarSeconds = stars["one"].GetInt();
        }

        levels.push_back(info);
    }

    if (levels.empty())
    {
        levels.push_back(defaultLevel(1, "levels/level_001.json", 35, 55, 80));
        levels.push_back(defaultLevel(2, "levels/level_002.json", 55, 85, 120));
        levels.push_back(defaultLevel(3, "levels/level_003.json", 80, 120, 170));
    }

    return levels;
}

int calculateStarCount(float elapsedSeconds, const LevelInfo& info)
{
    if (elapsedSeconds <= info.threeStarSeconds)
    {
        return 3;
    }
    if (elapsedSeconds <= info.twoStarSeconds)
    {
        return 2;
    }
    return 1;
}
