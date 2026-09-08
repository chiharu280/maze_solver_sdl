# Maze Solver 协作指南

## 项目定位

这是一个 C11 + SDL2 的迷宫求解与动画可视化项目。程序加载文本迷宫，使用 BFS 寻找从 `S` 到 `E` 的四方向最短路径，并在 SDL2 窗口中播放老鼠移动动画。递归 DFS 仍被保留，作为算法学习和对比实现。

当前 `main` 已是稳定的 `v1.0.0` 基线（标签：`v1.0.0`）。后续重构应采用小步、可验证的提交；不要同时重写数据模型、求解器和 SDL 前端。

## 目录与职责

| 路径 | 职责 |
| --- | --- |
| `src/main.c` | 编排流程：加载迷宫、定位 `S`、运行 BFS、启动可视化。 |
| `src/maze.c` / `include/maze.h` | 迷宫文件解析、完整性校验、`S`/`E` 查找；维护全局 `maze`、`ROWS`、`COLS`。 |
| `src/generator.c` / `include/generator.h` | 非递归随机 DFS 完美迷宫生成器；可生成内存网格或写入文本文件。 |
| `src/solver.c` / `include/solver.h` | DFS、BFS 与共享路径缓冲区。 |
| `src/visualize.c` / `include/visualize.h` | SDL2 初始化、贴图、路径动画和资源清理。 |
| `assets/` | 运行时必需资源：`maze.txt`、`mouse.bmp`、`cheese.bmp`。 |
| `tools/maze_gen.c` | C 版迷宫生成器命令行入口。 |
| `python/maze_gen.py` | 已废弃的旧版 Python 生成器，仅保留作为参考。 |
| `Makefile` | Linux/WSL 原生构建与 MinGW-w64 Windows 交叉编译。 |
| `lib/` | 随项目提供的 SDL2/MinGW 开发文件；除非明确更新依赖，否则不要修改。 |

## 关键数据与接口约定

- 迷宫文件首行为 `列数 行数`，后面必须恰好有对应数量的行和列；上限均为 100。
- 单元格只允许 `#`、空格、`S`、`E`，且 `S` 和 `E` 必须各出现一次。读取器接受 LF 与 CRLF，拒绝非空白的额外迷宫数据。
- 坐标以 `(x, y)` 表示，其中 `x` 为列、`y` 为行；访问网格必须使用 `maze[y][x]`。
- 默认求解器是 `solve_maze_bfs()`。在四方向、无权图中它必须保持最短路保证，且不得修改原始迷宫。
- `path_x`、`path_y` 的路径顺序固定为 `E -> ... -> S`；可视化层通过反向遍历将动画从 `S` 播放到 `E`。任何新求解器都必须遵守此契约，或同步调整公共接口和调用方。
- DFS 的 `solve_maze()` 会临时以 `.` 标记搜索过程，不能作为“迷宫保持不变”的实现范例；DFS 不保证最短路。
- 渲染使用独立的 `revealed` 覆盖层展示路线，避免为动画修改 `maze`。动画等待期间必须继续处理 `SDL_QUIT`；SDL 资源应走统一清理路径。
- 资源路径目前相对于项目/运行目录：`assets/maze.txt`、`assets/mouse.bmp`、`assets/cheese.bmp`。移动可执行文件或调整打包布局时须一起更新或保留该目录结构。

## 开发与验证

- 保持 C11，并启用（或不削弱）`-Wall -Wextra -Wpedantic`。
- 在 Linux、WSL 或已正确安装 SDL2 的环境中，使用 `make` 构建、`./maze_solver` 运行。改动后至少用默认迷宫完成一次动画验证。
- 在 Linux/WSL 为 64 位 Windows 交叉编译：先安装 `mingw-w64`，随后运行 `make windows`；需要发行目录时运行 `make package-win`。若本机 SDL2 包位置不同，先核对 `SDL2_WIN`，并确认 DLL 实际拷贝路径。
- 使用 `make generator` 构建 C 版生成器，并从项目根目录运行 `./maze_generator`。自定义尺寸可运行 `./maze_generator <宽度> <高度> [输出文件 [随机种子]]`；宽高必须为不大于 100 的奇数，并应重新运行程序验证生成文件。
- 除正常迷宫外，涉及解析或求解的改动应覆盖：非法尺寸、行宽不符、非法字符、重复/缺失 `S` 或 `E`、无路径、最大 `100 x 100` 迷宫，以及路径首尾和相邻性。
- 当前历史记录表明原生 Windows 环境未必配置了 GCC/Make/Python；不要把“本机无法构建”误判为代码失败。应报告实际使用的平台、工具链和执行过的命令。

## Git 与变更边界

- `main` 用作稳定发布基线；较大的功能或重构从 `codex/` 前缀的新分支开始。
- 先检查 `git status` 和 `git diff`，保留用户已有的未提交改动；不要使用 `git push --force`。
- `.vscode/`、`dist/`、构建产物和二进制已忽略，不应重新纳入版本控制。
- 优先修改项目自有代码与文档；`lib/` 是第三方内容，避免进行格式化、批量替换或无关提交。
- 修改公开头文件、路径格式或迷宫文件规则时，必须同步检查 `README.md` 的中英文说明。
