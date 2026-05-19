#include "GameModel.h"
#include <cstdlib>

bool GameModel::reset(const LevelConfig& config)
{
    _tableau.clear();
    _stock = config.stock;
    _currentWaste = config.initialWaste;
    _hasCurrentWaste = true;
    _undoStack.clear();

    for (const auto& cardConfig : config.tableau)
    {
        TableauCardState state;
        state.config = cardConfig;
        state.removed = false;
        _tableau.push_back(state);
    }

    rebuildIndex();
    return true;
}

bool GameModel::canMatch(const CardData& card) const
{
    if (!_hasCurrentWaste)
    {
        return false;
    }
    const int diff = std::abs(rankValue(card.rank) - rankValue(_currentWaste.rank));
    return diff == 1;
}

GameStateSnapshot GameModel::createSnapshot() const
{
    GameStateSnapshot snapshot;
    snapshot.tableau = _tableau;
    snapshot.stock = _stock;
    snapshot.currentWaste = _currentWaste;
    snapshot.hasCurrentWaste = _hasCurrentWaste;
    return snapshot;
}

bool GameModel::isTableauCardAvailable(const std::string& id) const
{
    const TableauCardState* state = findTableauCard(id);
    if (!state || state->removed)
    {
        return false;
    }

    for (const auto& coverId : state->config.coveredBy)
    {
        const TableauCardState* cover = findTableauCard(coverId);
        if (cover && !cover->removed)
        {
            return false;
        }
    }
    return true;
}

bool GameModel::matchTableauCard(const std::string& id)
{
    TableauCardState* state = findTableauCard(id);
    if (!state || state->removed || !isTableauCardAvailable(id) || !canMatch(state->config.card))
    {
        return false;
    }

    saveSnapshot();
    state->removed = true;
    _currentWaste = state->config.card;
    _hasCurrentWaste = true;
    return true;
}

bool GameModel::drawStock()
{
    if (_stock.empty())
    {
        return false;
    }

    saveSnapshot();
    _currentWaste = _stock.back();
    _stock.pop_back();
    _hasCurrentWaste = true;
    return true;
}

bool GameModel::undo()
{
    if (_undoStack.empty())
    {
        return false;
    }

    const GameStateSnapshot snapshot = _undoStack.back();
    _undoStack.pop_back();

    _tableau = snapshot.tableau;
    _stock = snapshot.stock;
    _currentWaste = snapshot.currentWaste;
    _hasCurrentWaste = snapshot.hasCurrentWaste;
    rebuildIndex();
    return true;
}

bool GameModel::isCleared() const
{
    for (const auto& state : _tableau)
    {
        if (!state.removed)
        {
            return false;
        }
    }
    return true;
}

TableauCardState* GameModel::findTableauCard(const std::string& id)
{
    auto it = _indexById.find(id);
    if (it == _indexById.end())
    {
        return nullptr;
    }
    return &_tableau[it->second];
}

const TableauCardState* GameModel::findTableauCard(const std::string& id) const
{
    auto it = _indexById.find(id);
    if (it == _indexById.end())
    {
        return nullptr;
    }
    return &_tableau[it->second];
}

void GameModel::saveSnapshot()
{
    GameStateSnapshot snapshot;
    snapshot.tableau = _tableau;
    snapshot.stock = _stock;
    snapshot.currentWaste = _currentWaste;
    snapshot.hasCurrentWaste = _hasCurrentWaste;
    _undoStack.push_back(snapshot);
}

void GameModel::rebuildIndex()
{
    _indexById.clear();
    for (size_t i = 0; i < _tableau.size(); ++i)
    {
        _indexById[_tableau[i].config.id] = i;
    }
}
