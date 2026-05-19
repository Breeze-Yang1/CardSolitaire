# 纸牌程序设计文档

本文档对应当前 Cocos2d-x 工程代码结构，用于说明程序模块划分、主要数据流，以及后续扩展卡牌和回退功能时应修改的位置。当前版本仍属于阶段性实现，后续若需求继续变化，可在此文档基础上补充。

## 1. 工程目标与当前范围

当前程序实现纸牌消除类玩法的基础版本。程序启动后先进入关卡选择页，玩家选择关卡后进入牌局。牌局中的桌面牌、初始底牌、手牌堆和覆盖关系均由 JSON 配置读取。玩家可以点击未被覆盖且点数相邻的桌面牌进行消除，也可以从手牌堆翻出新底牌。有效操作会写入回退栈，因此当前已支持多步回退。

当前规则边界如下：

- `A` 与 `K` 不链接。
- 不包含 Joker 或 Wild 牌。
- 花色只影响显示颜色和图案，不参与匹配判断。
- 当前支持回退，不支持重做。
- 通关后按关卡配置中的时间阈值显示星级。

## 2. 代码结构

### 2.1 卡牌基础类型

`Classes/CardTypes.h` 和 `Classes/CardTypes.cpp` 定义卡牌的基础数据结构：

- `CardRank`：点数枚举，目前包含 `A` 到 `K`。
- `CardSuit`：花色枚举，目前包含黑桃、红心、方块和梅花。
- `CardData`：单张牌的数据，由点数和花色组成。
- `parseRank`、`parseSuit`：把 JSON 中的字符串转换为枚举。
- `rankToString`、`suitToString`：把枚举转换为资源路径中使用的字符串。
- `numberAssetPath`、`suitAssetPath`：根据卡牌数据拼出点数图和花色图路径。

该层只描述“卡牌是什么”和“资源怎么找”，不直接处理牌局规则。

### 2.2 关卡配置读取

`Classes/LevelConfig.h` 和 `Classes/LevelConfig.cpp` 负责读取单个关卡文件。当前 JSON 结构包括：

- `initialWaste`：开局底牌。
- `stock`：手牌堆。程序翻牌时从数组末尾取牌。
- `tableau`：桌面牌列表。
- `coveredBy`：当前牌被哪些牌覆盖。只有覆盖牌都被移除后，该牌才可点击。

`Resources/levels/index.json` 负责关卡列表和星级时间阈值，读取逻辑在 `LevelCatalog` 中。

### 2.3 牌局模型

`Classes/GameModel.h` 和 `Classes/GameModel.cpp` 管理纯牌局状态，主要成员包括：

- `_tableau`：桌面牌当前状态。
- `_stock`：手牌堆。
- `_currentWaste`：当前底牌。
- `_undoStack`：回退快照栈。

当前有效操作主要有两类：

- `matchTableauCard`：消除桌面牌，并把该牌变成当前底牌。
- `drawStock`：从手牌堆翻出一张牌，替换当前底牌。

这两个函数在修改状态前都会调用 `saveSnapshot`。因此每次回退时，`undo` 只需要恢复最近一次快照即可。

### 2.4 场景与显示

`Classes/LevelSelectScene.*` 实现主页关卡选择。

`Classes/GameScene.*` 负责游戏界面、点击事件、动画和模型同步。它不直接保存规则结果，而是调用 `GameModel` 完成状态变化，再根据模型状态刷新牌面显示。

`Classes/CardSprite.*` 只负责单张牌的显示，包括牌面、点数、花色和背面状态。

## 3. 新增一个卡牌的扩展方式

这里的“新增一个卡牌”需要先区分两种情况。

### 3.1 只是在关卡里多放一张已有点数和花色的牌

如果新增的是当前已有的普通牌，例如 `7 heart` 或 `Q spade`，不需要修改 C++ 代码，只需要改关卡 JSON。

示例：

```json
{
  "id": "new_card_01",
  "rank": "7",
  "suit": "heart",
  "x": 540,
  "y": 1200,
  "coveredBy": []
}
```

需要注意：

- `id` 在同一关卡内必须唯一。
- `rank` 必须是当前 `parseRank` 支持的字符串。
- `suit` 必须是当前 `parseSuit` 支持的字符串。
- 如果该牌压住下层牌，下层牌的 `coveredBy` 中要写入该牌的 `id`。
- 坐标仍按当前设计分辨率 `1080 * 2080` 布置。

### 3.2 新增一种当前代码不认识的牌

如果要新增 Joker、特殊功能牌或新的花色，就需要同时改数据、解析、资源和规则。

建议修改顺序如下：

1. 在 `CardTypes.h` 中扩展 `CardRank` 或 `CardSuit`。
2. 在 `CardTypes.cpp` 中同步修改 `parseRank`、`parseSuit`、`rankToString`、`suitToString`。
3. 在 `Resources/number/` 或 `Resources/suits/` 中补齐对应图片资源。
4. 如果新增卡牌仍按普通点数匹配，需要检查 `rankValue` 是否能返回合理数值。
5. 如果新增卡牌有特殊规则，不建议直接塞进 `GameScene`，应优先在 `GameModel::canMatch` 或新的规则函数中处理。
6. 在关卡 JSON 中使用新字符串，并运行程序确认配置能被 `LevelConfigLoader` 正常读取。

例如未来要加入 Joker，不能只在 JSON 中写 `"rank": "Joker"`。如果 `parseRank` 没有识别它，关卡读取会失败。还需要明确 Joker 的匹配规则：是任意匹配、只匹配指定牌，还是作为特殊道具触发效果。这个判断应放在模型层，而不是写在点击事件里。

## 4. 新增一种回退功能的扩展方式

当前回退机制采用“状态快照”方案。每次有效操作前调用 `saveSnapshot`，把桌面牌、手牌堆、当前底牌等完整状态压入 `_undoStack`。点击回退时，`undo` 恢复最近一次快照。

这种方案的优点是规则简单，适合当前的小规模牌局。新增普通操作时，只要该操作改变了牌局状态，一般只需要在修改前保存快照即可。

### 4.1 新增普通可回退操作

如果未来新增一种操作，例如“洗牌手牌堆”“交换两张桌面牌”“临时移除一张障碍牌”，建议按下面流程处理：

1. 在 `GameModel` 中新增一个公开函数，例如 `shuffleStock` 或 `swapTableauCards`。
2. 在函数开头先校验操作是否合法。
3. 校验通过后调用 `saveSnapshot`。
4. 再修改 `_tableau`、`_stock`、`_currentWaste` 等状态。
5. 在 `GameScene` 中新增点击入口或按钮逻辑。
6. 操作成功后调用 `syncFromModel` 或新增对应动画。

示意结构如下：

```cpp
bool GameModel::newOperation()
{
    if (!operationIsValid)
    {
        return false;
    }

    saveSnapshot();
    // 修改牌局状态
    return true;
}
```

只要该操作能被完整快照覆盖，`undo` 函数本身通常不需要改。

### 4.2 新增一种带专属动画的回退类型

当前 `GameScene::animateUndo` 会比较回退前后的快照：

- 如果发现某张桌面牌从“已移除”变回“未移除”，就播放桌面牌恢复动画。
- 如果发现手牌堆数量恢复，就播放翻牌回退动画。
- 如果无法识别具体差异，就直接同步模型状态。

如果新增的回退类型需要单独动画，需要在 `GameScene` 中增加识别逻辑和动画函数。例如新增“交换两张牌”的回退动画，可以按下面方式处理：

1. 在 `GameScene::animateUndo` 中比较 `beforeUndo` 和 `afterUndo`。
2. 判断是否属于新操作造成的状态变化。
3. 新增 `animateUndoSwapCards` 之类的动画函数。
4. 动画结束后调用 `syncFromModel`，并把 `_isAnimating` 设回 `false`。

如果仅靠前后快照难以判断操作类型，就不建议继续用差异猜测。可以把 `_undoStack` 从单纯的 `GameStateSnapshot` 扩展为带类型的结构，例如：

```cpp
enum class UndoActionType
{
    MatchTableau,
    DrawStock,
    SwapCards
};

struct UndoRecord
{
    UndoActionType type;
    GameStateSnapshot snapshot;
};
```

这样 `GameModel` 在保存快照时同时记录操作类型，`GameScene` 回退时就能按类型选择动画。这个改动会比当前快照栈稍复杂，但更适合后续加入更多特殊操作。

## 5. 扩展时的约束

后续扩展应尽量保持当前分层：

- 新卡牌的数据定义放在 `CardTypes`。
- 关卡中出现哪些牌、坐标和覆盖关系放在 JSON。
- 牌局规则放在 `GameModel`。
- 动画和界面交互放在 `GameScene`。
- 单张牌显示放在 `CardSprite`。

不建议把新规则直接写在触摸坐标判断中。这样短期可以跑通，但后续回退、动画、关卡配置和测试都会变得难维护。

## 6. 当前仍可补充的内容

当前文档已经能说明主要代码结构和扩展路径，但还可以继续补充以下内容：

- 更完整的类图或流程图。
- 关卡 JSON 字段说明表。
- 操作时序图，例如点击桌面牌到动画结束的完整过程。
- 新增特殊牌后的真实规则说明。
- 自动化测试方案。
