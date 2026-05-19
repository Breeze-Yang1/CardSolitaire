#include "LevelConfig.h"
#include "json/document.h"

USING_NS_CC;

namespace
{
bool readCard(const rapidjson::Value& value, CardData* outCard, std::string* error)
{
    if (!value.IsObject() || !value.HasMember("rank") || !value.HasMember("suit"))
    {
        if (error) *error = "card object must contain rank and suit";
        return false;
    }

    if (!value["rank"].IsString() || !value["suit"].IsString())
    {
        if (error) *error = "rank and suit must be string";
        return false;
    }

    CardRank rank;
    CardSuit suit;
    if (!parseRank(value["rank"].GetString(), &rank))
    {
        if (error) *error = "invalid rank: " + std::string(value["rank"].GetString());
        return false;
    }
    if (!parseSuit(value["suit"].GetString(), &suit))
    {
        if (error) *error = "invalid suit: " + std::string(value["suit"].GetString());
        return false;
    }

    outCard->rank = rank;
    outCard->suit = suit;
    return true;
}
}

bool LevelConfigLoader::loadFromFile(const std::string& path, LevelConfig* outConfig, std::string* error)
{
    if (!outConfig)
    {
        if (error) *error = "outConfig is null";
        return false;
    }

    const std::string content = FileUtils::getInstance()->getStringFromFile(path);
    if (content.empty())
    {
        if (error) *error = "cannot read level file: " + path;
        return false;
    }

    rapidjson::Document doc;
    doc.Parse<0>(content.c_str());
    if (doc.HasParseError() || !doc.IsObject())
    {
        if (error) *error = "invalid json level file: " + path;
        return false;
    }

    LevelConfig config;
    config.designSize = Size(1080.0f, 2080.0f);
    if (doc.HasMember("designSize") && doc["designSize"].IsObject())
    {
        const rapidjson::Value& size = doc["designSize"];
        if (size.HasMember("width") && size.HasMember("height"))
        {
            config.designSize = Size(size["width"].GetFloat(), size["height"].GetFloat());
        }
    }

    if (!doc.HasMember("initialWaste") || !readCard(doc["initialWaste"], &config.initialWaste, error))
    {
        return false;
    }

    if (doc.HasMember("stock") && doc["stock"].IsArray())
    {
        for (rapidjson::SizeType i = 0; i < doc["stock"].Size(); ++i)
        {
            CardData card;
            if (!readCard(doc["stock"][i], &card, error))
            {
                return false;
            }
            config.stock.push_back(card);
        }
    }

    if (!doc.HasMember("tableau") || !doc["tableau"].IsArray())
    {
        if (error) *error = "tableau array is required";
        return false;
    }

    for (rapidjson::SizeType i = 0; i < doc["tableau"].Size(); ++i)
    {
        const rapidjson::Value& item = doc["tableau"][i];
        if (!item.IsObject() || !item.HasMember("id") || !item["id"].IsString())
        {
            if (error) *error = "tableau item id is required";
            return false;
        }
        if (!item.HasMember("x") || !item.HasMember("y"))
        {
            if (error) *error = "tableau item position is required";
            return false;
        }

        TableauCardConfig cardConfig;
        cardConfig.id = item["id"].GetString();
        if (!readCard(item, &cardConfig.card, error))
        {
            return false;
        }
        cardConfig.position = Vec2(item["x"].GetFloat(), item["y"].GetFloat());

        if (item.HasMember("coveredBy") && item["coveredBy"].IsArray())
        {
            for (rapidjson::SizeType j = 0; j < item["coveredBy"].Size(); ++j)
            {
                if (item["coveredBy"][j].IsString())
                {
                    cardConfig.coveredBy.push_back(item["coveredBy"][j].GetString());
                }
            }
        }

        config.tableau.push_back(cardConfig);
    }

    *outConfig = config;
    return true;
}
