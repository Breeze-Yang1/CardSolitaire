# CardSolitaire

纸牌程序设计需求一的 Cocos2d-x 临时实现工程。

## 当前功能

- 支持主页关卡选择，当前配置 3 个关卡，难度递增。
- 关卡牌阵从 `Resources/levels/*.json` 读取，不写死在代码中。
- 通关后记录用时，并按关卡配置显示 1-3 星。
- 支持按点数相邻关系匹配桌面牌和当前底牌，花色不参与判断。
- `A` 与 `K` 不链接。
- 不包含 Joker/Wild 逻辑。
- 支持手牌堆翻牌替换当前底牌。
- 支持多步回退，当前不包含重做功能。
- 消除后是否翻开下层牌，由配置中的 `coveredBy` 覆盖关系动态计算。

## 目录说明

- `Classes/`：Cocos2d-x 业务代码。
- `Resources/`：牌面资源、花色资源和关卡配置。
- `Resources/levels/index.json`：关卡列表与星级时间阈值。
- `docs/temporary_decisions.md`：当前阶段临时技术决策记录。
- `tools/fetch_cocos2dx_317.ps1`：下载 Cocos2d-x 3.17.2 的辅助脚本。

## 构建说明

本仓库不提交 `third_party/` 和 `build/` 目录。首次构建前需要准备 Cocos2d-x 3.17.2：

```powershell
.\tools\fetch_cocos2dx_317.ps1
$env:COCOS2DX_ROOT = (Resolve-Path .\third_party\cocos2d-x-3.17.2).Path
```

然后使用 CMake 生成并构建 Win32 Debug 工程：

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A Win32 -DCOCOS2DX_ROOT="$env:COCOS2DX_ROOT"
cmake --build build --config Debug --target CardSolitaire -- /m
```

构建产物位于：

```text
build/bin/Debug/CardSolitaire.exe
```

当前验证环境为 Windows、Visual Studio 2022 Build Tools、CMake 和 Cocos2d-x 3.17.2。64 位构建暂未作为当前验证目标。
