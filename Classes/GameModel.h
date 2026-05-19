#ifndef __GAME_MODEL_H__
#define __GAME_MODEL_H__

#include "LevelConfig.h"
#include <unordered_map>

struct TableauCardState
{
    TableauCardConfig config;
    bool removed = false;
};

struct GameStateSnapshot
{
    std::vector<TableauCardState> tableau;
    std::vector<CardData> stock;
    CardData currentWaste;
    bool hasCurrentWaste = false;
};

class GameModel
{
public:
    bool reset(const LevelConfig& config);

    const std::vector<TableauCardState>& getTableau() const { return _tableau; }
    const std::vector<CardData>& getStock() const { return _stock; }
    const CardData& getCurrentWaste() const { return _currentWaste; }
    bool hasUndo() const { return !_undoStack.empty(); }
    size_t getUndoCount() const { return _undoStack.size(); }
    GameStateSnapshot createSnapshot() const;

    bool canMatch(const CardData& card) const;
    bool isTableauCardAvailable(const std::string& id) const;
    bool matchTableauCard(const std::string& id);
    bool drawStock();
    bool undo();
    bool isCleared() const;

private:
    TableauCardState* findTableauCard(const std::string& id);
    const TableauCardState* findTableauCard(const std::string& id) const;
    void saveSnapshot();
    void rebuildIndex();

private:
    std::vector<TableauCardState> _tableau;
    std::unordered_map<std::string, size_t> _indexById;
    std::vector<CardData> _stock;
    CardData _currentWaste;
    bool _hasCurrentWaste = false;
    std::vector<GameStateSnapshot> _undoStack;
};

#endif
