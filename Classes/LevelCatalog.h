#ifndef __LEVEL_CATALOG_H__
#define __LEVEL_CATALOG_H__

#include <string>
#include <vector>

struct LevelInfo
{
    int id = 1;
    std::string name;
    std::string file;
    int threeStarSeconds = 45;
    int twoStarSeconds = 70;
    int oneStarSeconds = 100;
};

class LevelCatalog
{
public:
    static std::vector<LevelInfo> loadLevels(const std::string& path);
};

int calculateStarCount(float elapsedSeconds, const LevelInfo& info);

#endif
