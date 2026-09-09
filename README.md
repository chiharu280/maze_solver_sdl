# Maze Solver 1.0 / 迷宫求解与可视化

[English](#english) | [中文](#中文)

## 中文

### 项目简介

Maze Solver 是一个使用 **C 和 SDL2** 编写的迷宫求解与动画可视化项目。程序从文本文件加载迷宫，使用 **广度优先搜索（BFS）** 求出从老鼠起点到奶酪终点的最短路径，并在 SDL2 窗口中播放老鼠移动的过程。

项目也保留了递归 **深度优先搜索（DFS）** 实现，便于后续进行算法对比。

### 功能

- 从文本文件加载并严格校验迷宫格式
- 使用 BFS 求解四方向、无权迷宫的最短路径
- 保留 DFS 求解器，供算法学习与比较使用
- 使用 SDL2 绘制墙体、路径、老鼠和奶酪
- 动画展示从起点到终点的移动过程
- 求解完成后提供“再来一次”或返回初始界面的选择
- 在动画期间保持窗口关闭事件可响应
- 提供开始求解、生成新迷宫、语言切换和退出游戏的开始界面
- 内置中英文界面，不依赖操作系统字体
- 窗口可自由缩放，菜单与迷宫保持正确比例和点击区域
- 使用 C 随机生成新的完美迷宫

### 项目结构

| 路径 | 说明 |
| --- | --- |
| `src/main.c` | 程序入口：加载迷宫、调用 BFS、启动可视化 |
| `src/app.c` | SDL2 窗口、渲染器、贴图及应用级资源生命周期 |
| `src/maze.c` | 迷宫文件读取、格式校验和标记查找 |
| `src/generator.c` | C 版随机完美迷宫生成器 |
| `src/menu.c` | 开始界面、内置位图字体、按钮绘制与交互 |
| `src/ui.c` | 中英文界面文本和 UTF-8 位图字体渲染 |
| `src/solver.c` | DFS、BFS 与路径记录逻辑 |
| `src/visualize.c` | 迷宫渲染和路径动画 |
| `include/` | 各模块的公共头文件 |
| `assets/maze.txt` | 默认迷宫 |
| `assets/mouse.bmp` | 老鼠贴图 |
| `assets/cheese.bmp` | 奶酪贴图 |
| `assets/ui_font.hex` | 界面所需的中文字形子集 |
| `tools/maze_gen.c` | C 版迷宫生成器命令行入口 |
| `python/maze_gen.py` | 已废弃的旧版 Python 生成器（保留参考） |
| `Makefile` | Linux/WSL 构建与 Windows 交叉编译规则 |

### 依赖

- C11 兼容编译器，例如 GCC
- SDL2 开发库

Ubuntu / Debian / WSL 可执行：

```bash
sudo apt install build-essential libsdl2-dev
```

### 构建与运行

在项目根目录执行：

```bash
make
./maze_solver
```

也可以直接使用 GCC：

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic src/*.c -Iinclude \
  $(sdl2-config --cflags --libs) -o maze_solver
./maze_solver
```

程序默认使用中文，并显示包含四个按钮的开始界面：

- `开始`：求解当前迷宫并播放动画
- `新迷宫`：打开宽高输入界面，确认后生成新迷宫并写入 `assets/maze.txt`
- `语言 中文`：在中文和英文之间切换，界面会立即更新
- `退出`：退出游戏

老鼠到达奶酪后，迷宫中央会显示完成窗口。选择“再来一次”会重新播放当前迷宫；选择“退出”或按 `Esc` 会返回初始界面。关闭 SDL 窗口才会退出整个应用。

尺寸页面点击 `WIDTH` 或 `HEIGHT` 后直接输入数字即可替换原值，`Tab` 切换输入框，退格删除数字。按 `Enter` 或点击 `GENERATE` 确认；`Esc` 或 `CANCEL` 返回菜单且不生成文件。宽高须为 `3` 到 `99` 的奇数，不能同时为 `3`。

窗口可以拖拽缩放。SDL2 会分别按照菜单和迷宫的逻辑画布等比例缩放，宽高比不同时自动留边。菜单支持 `Enter`/空格开始、`N` 生成迷宫、`L` 切换语言、`Q`/`Esc` 退出；动画期间按 `Esc` 可返回菜单。

### 从 Linux / WSL 交叉编译 Windows 版本

Makefile 保留了面向 **64 位 Windows** 的 MinGW-w64 交叉编译目标。先在 Ubuntu、Debian 或 WSL 中安装交叉编译器：

```bash
sudo apt install mingw-w64
```

然后在项目根目录构建 Windows 可执行文件：

```bash
make windows
```

生成结果为：

```text
dist/maze_solver.exe
```

若要同时复制 SDL2 运行时 DLL 和 `assets/` 资源目录，执行：

```bash
make package-win
```

完成后将整个 `dist/` 目录复制到 Windows 机器，并从该目录启动 `maze_solver.exe`。Makefile 默认使用 `x86_64-w64-mingw32-gcc` 和项目内的 `lib/x86_64-w64-mingw32` SDL2 开发包；若本地 SDL2 包路径不同，请先调整 `SDL2_WIN`。当前链接选项包含 `-mwindows`，因此 Windows 版本默认不显示控制台窗口。

### 迷宫文件格式

迷宫文件的第一行是：

```text
列数 行数
```

之后必须紧跟指定行数的迷宫内容。每一行必须恰好包含指定列数的字符，只允许使用：

| 字符 | 含义 |
| --- | --- |
| `#` | 墙体，不可通行 |
| 空格 | 通路 |
| `S` | 起点，且必须唯一 |
| `E` | 终点，且必须唯一 |

一个 `5 × 3` 的示例：

```text
5 3
#####
#S E#
#####
```

文件尺寸最大为 `100 × 100`。程序会拒绝尺寸错误、行长度错误、非法字符、多余迷宫数据，或缺少/重复 `S`、`E` 的文件。

### 算法说明

#### BFS（默认）

`solve_maze_bfs()` 使用队列逐层搜索。对于本项目这种每步代价相同、只能上下左右移动的迷宫，BFS 保证返回最短路径。

#### DFS（保留实现）

`solve_maze()` 使用递归深度优先搜索和回溯。它可找到一条可行路径，但不保证路径最短。该实现主要用于算法对比与学习。

两个求解器都会将路径记录在 `path_x`、`path_y` 和 `path_len` 中，顺序为“终点到起点”；可视化模块会反向读取，从起点播放至终点。

### 生成新迷宫

先构建 C 版生成器，然后从项目根目录生成默认的 `91 × 91` 迷宫：

```bash
make generator
./maze_generator
```

默认命令会覆盖 `assets/maze.txt`。生成器本身不依赖 SDL2，只需要 C11 编译器。

也可以指定宽度、高度、输出文件和可选的随机种子：

```bash
./maze_generator 51 41 assets/maze.txt 12345
```

宽度和高度必须是 `3` 到 `100` 范围内的奇数（因此实际最大值为 `99`）；`3 × 3` 会因起点和终点重叠而被拒绝。生成器采用非递归随机 DFS，输出保证边界封闭、所有通路连通且无环，并直接使用读取器所要求的文件格式。指定相同尺寸和相同种子可复现完全相同的迷宫。

运行生成器自动化测试：

```bash
make test-generator
```

菜单、按钮事件、SDL 生命周期及逻辑缩放可使用无显示器测试验证：

```bash
make test-ui
```

旧的 `python/maze_gen.py` 已废弃，但仍保留用于参考，不参与当前构建或运行流程。

### 当前开发状态与问题记录

`dev` 分支已经完成 SDL 应用生命周期拆分、C 版完美迷宫生成器、三按钮开始菜单、动画返回菜单，以及可缩放逻辑画布。当前记录以下界面问题，作为后续修改和回归测试依据：

1. **已修复：**`NEW MAZE` 现会打开尺寸输入界面，支持鼠标切换宽高输入框、数字输入、退格、`Tab` 切换、确认和取消，并在生成前校验奇数范围。
2. **已修复：** 缩放后按钮失效源于对 SDL 已转换的鼠标事件再次进行逻辑坐标换算。菜单现直接使用 SDL 提供的逻辑事件坐标，并加入非等比例缩放后的按钮点击回归测试。

### 开发约定

- 源码使用 C11，并默认启用 `-Wall -Wextra -Wpedantic`。
- BFS 不修改原始迷宫；SDL2 使用单独的路径覆盖层绘制动画。
- SDL2 初始化、动画播放和资源清理相互独立；资源在应用退出时统一释放。
- 修改后建议重新执行 `make` 并用默认迷宫做一次完整动画验证。

---

## English

### Overview

Maze Solver is a **C and SDL2** project for solving and visualizing grid mazes. It loads a maze from a text file, uses **breadth-first search (BFS)** to find the shortest route from the mouse to the cheese, and animates that route in an SDL2 window.

A recursive **depth-first search (DFS)** implementation is also kept in the project for learning and algorithm comparison.

### Features

- Strict text-maze loading and validation
- Shortest-path BFS for four-direction, unweighted mazes
- Retained DFS implementation for comparison
- SDL2 rendering for walls, route, mouse, and cheese
- Animated traversal from start to destination
- Completion dialog with replay and return-to-menu actions
- Responsive close events during animation
- Start screen with solve, new-maze, language, and quit actions
- Built-in Chinese and English UI without a system-font dependency
- Freely resizable window with correctly scaled screens and hit targets
- C-based random perfect-maze generator

### Project layout

| Path | Purpose |
| --- | --- |
| `src/main.c` | Entry point: load, solve with BFS, and visualize |
| `src/app.c` | SDL2 window, renderer, textures, and application resource lifetime |
| `src/maze.c` | Maze parsing, validation, and marker lookup |
| `src/generator.c` | C random perfect-maze generator |
| `src/menu.c` | Start screen, built-in bitmap font, button rendering, and input |
| `src/ui.c` | Localized UI strings and UTF-8 bitmap-font rendering |
| `src/solver.c` | DFS, BFS, and path storage |
| `src/visualize.c` | Maze rendering and route animation |
| `include/` | Public module headers |
| `assets/maze.txt` | Default maze |
| `assets/mouse.bmp` | Mouse sprite |
| `assets/cheese.bmp` | Cheese sprite |
| `assets/ui_font.hex` | Chinese glyph subset used by the UI |
| `tools/maze_gen.c` | Command-line entry point for the C generator |
| `python/maze_gen.py` | Deprecated legacy Python generator, retained for reference |
| `Makefile` | Linux/WSL build and Windows cross-build rules |

### Requirements

- A C11-compatible compiler, such as GCC
- SDL2 development files

On Ubuntu, Debian, or WSL:

```bash
sudo apt install build-essential libsdl2-dev
```

### Build and run

From the project root:

```bash
make
./maze_solver
```

Or compile directly:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic src/*.c -Iinclude \
  $(sdl2-config --cflags --libs) -o maze_solver
./maze_solver
```

The application defaults to Chinese and opens with four menu buttons:

- `START`: solve and animate the current maze
- `NEW MAZE`: open the width/height input screen, then generate and write the confirmed maze to `assets/maze.txt`
- `LANGUAGE ENGLISH`: switch between Chinese and English; the screen updates immediately
- `QUIT`: exit the application

When the mouse reaches the cheese, a centered completion dialog offers `PLAY AGAIN` to replay the current maze and `EXIT` to return to the initial menu. Closing the SDL window still exits the application.

On the size screen, click `WIDTH` or `HEIGHT` and type digits to replace its value. Use `Tab` to switch fields and Backspace to delete digits. `Enter` or `GENERATE` confirms; `Esc` or `CANCEL` returns without generating a file. Both dimensions must be odd values from `3` to `99`, and cannot both be `3`.

The window can be resized freely. SDL2 scales the menu and maze logical canvases proportionally and letterboxes them when their aspect ratios differ. Press `Enter`/Space to start, `N` for a new maze, `L` to switch language, and `Q`/`Esc` to quit from the menu. During animation, `Esc` returns to the menu.

### Cross-compile a Windows build from Linux / WSL

The Makefile retains a MinGW-w64 cross-compilation target for **64-bit Windows**. First install the cross-compiler on Ubuntu, Debian, or WSL:

```bash
sudo apt install mingw-w64
```

Then build the Windows executable from the project root:

```bash
make windows
```

The executable is written to:

```text
dist/maze_solver.exe
```

To also copy the SDL2 runtime DLL and the `assets/` directory, run:

```bash
make package-win
```

Copy the complete `dist/` directory to a Windows machine and launch `maze_solver.exe` from that directory. By default, the Makefile uses `x86_64-w64-mingw32-gcc` and the bundled SDL2 package at `lib/x86_64-w64-mingw32`. Update `SDL2_WIN` first if the local SDL2 package is stored elsewhere. The current link flags include `-mwindows`, so the Windows build does not open a console window by default.

### Maze file format

The first line contains:

```text
columns rows
```

It must be followed by exactly that many maze rows, each with exactly the declared number of characters. Valid cell characters are:

| Character | Meaning |
| --- | --- |
| `#` | Wall; cannot be crossed |
| Space | Open path |
| `S` | Unique start cell |
| `E` | Unique destination cell |

Example, a `5 × 3` maze:

```text
5 3
#####
#S E#
#####
```

The maximum supported size is `100 × 100`. Invalid dimensions, row lengths, cell characters, extra maze data, or missing/duplicate `S` and `E` markers are rejected.

### Algorithms

#### BFS (default)

`solve_maze_bfs()` uses a queue to explore one distance layer at a time. For an unweighted maze with four-direction movement, it guarantees a shortest path.

#### DFS (retained implementation)

`solve_maze()` uses recursive depth-first search and backtracking. It can find a valid route, but it does not guarantee the shortest one. It is retained for study and comparison.

Both solvers store coordinates in `path_x`, `path_y`, and `path_len`, ordered from destination to start. The renderer reads that sequence in reverse to animate from start to destination.

### Generate a new maze

Build the C generator and run it from the project root to create the default `91 × 91` maze:

```bash
make generator
./maze_generator
```

The default command overwrites `assets/maze.txt`. The generator itself only requires a C11 compiler and does not depend on SDL2.

You can also specify the width, height, output file, and an optional random seed:

```bash
./maze_generator 51 41 assets/maze.txt 12345
```

Width and height must be odd values from `3` through `100` (so the effective maximum is `99`); `3 × 3` is rejected because its start and end would overlap. The generator uses iterative randomized DFS and writes a closed-border, connected, acyclic maze directly in the loader's file format. Reusing the same dimensions and seed reproduces the same maze.

Run the generator test suite with:

```bash
make test-generator
```

Run the headless menu, input, SDL lifecycle, and logical-scaling tests with:

```bash
make test-ui
```

The old `python/maze_gen.py` is deprecated and retained for reference only. It is not part of the current build or runtime flow.

### Current development status and issue log

The `dev` branch now includes the separated SDL application lifecycle, C perfect-maze generator, three-button start menu, return-to-menu animation flow, and resizable logical canvases. The following UI issues are recorded for implementation and regression testing:

1. **Resolved:** `NEW MAZE` now opens a dimension-entry screen with mouse field selection, numeric input, Backspace, `Tab`, confirm/cancel controls, and odd-range validation before generation.
2. **Resolved:** Resized clicks failed because already-transformed SDL mouse events were converted to logical coordinates a second time. The menu now consumes SDL's logical event coordinates directly, with a regression test covering button clicks after non-proportional resizing.

### Development notes

- The project targets C11 and enables `-Wall -Wextra -Wpedantic` by default.
- BFS does not modify the original maze; SDL2 renders the route through a separate overlay.
- SDL2 initialization, animation, and cleanup are separate; resources are released once when the application exits.
- After a change, rebuild with `make` and run the default maze through a complete animation.
