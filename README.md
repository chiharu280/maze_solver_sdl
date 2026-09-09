# Maze Solver 1.0 / 迷宫求解与可视化

[English](#english) | [中文](#中文) | [Français](#français) | [日本語](#日本語)

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
- 内置中文、英文和法文界面，不依赖操作系统字体
- 窗口可自由缩放，菜单与迷宫保持正确比例和点击区域
- 使用 C 随机生成新的完美迷宫

### 项目结构

| 路径 | 说明 |
| --- | --- |
| `src/main.c` | 程序入口：加载迷宫、调用 BFS、启动可视化 |
| `src/app.c` | SDL2 窗口、渲染器、贴图及应用级资源生命周期 |
| `src/maze.c` | 迷宫文件读取、格式校验和标记查找 |
| `src/generator.c` | C 版随机完美迷宫生成器 |
| `src/menu.c` | 开始界面、语言选择界面、按钮绘制与交互 |
| `src/ui.c` | 中英法界面文本和 UTF-8 位图字体渲染 |
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
- `语言`：进入独立语言选择页，可选择中文、English 或 Français
- `退出`：退出游戏

老鼠到达奶酪后，迷宫中央会显示完成窗口。选择“再来一次”会重新播放当前迷宫；选择“退出”或按 `Esc` 会返回初始界面。关闭 SDL 窗口才会退出整个应用。

尺寸页面点击 `WIDTH` 或 `HEIGHT` 后直接输入数字即可替换原值，`Tab` 切换输入框，退格删除数字。按 `Enter` 或点击 `GENERATE` 确认；`Esc` 或 `CANCEL` 返回菜单且不生成文件。宽高须为 `3` 到 `99` 的奇数，不能同时为 `3`。

窗口可以拖拽缩放。SDL2 会分别按照菜单和迷宫的逻辑画布等比例缩放，宽高比不同时自动留边。菜单支持 `Enter`/空格开始、`N` 生成迷宫、`L` 打开语言选择页、`Q`/`Esc` 退出；语言页也可按 `C`、`E`、`F` 选择中文、英文、法文。动画期间按 `Esc` 可返回菜单。

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

当前程序已经具备 SDL 应用生命周期拆分、C 版完美迷宫生成器、四按钮开始菜单、三语言选择、完成弹窗，以及可缩放逻辑画布。以下界面问题保留为回归测试记录：

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
- Built-in Chinese, English, and French UI without a system-font dependency
- Freely resizable window with correctly scaled screens and hit targets
- C-based random perfect-maze generator

### Project layout

| Path | Purpose |
| --- | --- |
| `src/main.c` | Entry point: load, solve with BFS, and visualize |
| `src/app.c` | SDL2 window, renderer, textures, and application resource lifetime |
| `src/maze.c` | Maze parsing, validation, and marker lookup |
| `src/generator.c` | C random perfect-maze generator |
| `src/menu.c` | Start screen, language selection, button rendering, and input |
| `src/ui.c` | Chinese, English, and French UI strings and bitmap-font rendering |
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
- `LANGUAGE`: open a separate screen and choose Chinese, English, or French
- `QUIT`: exit the application

When the mouse reaches the cheese, a centered completion dialog offers `PLAY AGAIN` to replay the current maze and `EXIT` to return to the initial menu. Closing the SDL window still exits the application.

On the size screen, click `WIDTH` or `HEIGHT` and type digits to replace its value. Use `Tab` to switch fields and Backspace to delete digits. `Enter` or `GENERATE` confirms; `Esc` or `CANCEL` returns without generating a file. Both dimensions must be odd values from `3` to `99`, and cannot both be `3`.

The window can be resized freely. SDL2 scales the menu and maze logical canvases proportionally and letterboxes them when their aspect ratios differ. Press `Enter`/Space to start, `N` for a new maze, `L` to open language selection, and `Q`/`Esc` to quit from the menu. On the language screen, `C`, `E`, and `F` select Chinese, English, and French. During animation, `Esc` returns to the menu.

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

The current application includes the separated SDL application lifecycle, C perfect-maze generator, four-button start menu, three-language selection, completion dialog, and resizable logical canvases. The following UI issues are recorded for regression testing:

1. **Resolved:** `NEW MAZE` now opens a dimension-entry screen with mouse field selection, numeric input, Backspace, `Tab`, confirm/cancel controls, and odd-range validation before generation.
2. **Resolved:** Resized clicks failed because already-transformed SDL mouse events were converted to logical coordinates a second time. The menu now consumes SDL's logical event coordinates directly, with a regression test covering button clicks after non-proportional resizing.

### Development notes

- The project targets C11 and enables `-Wall -Wextra -Wpedantic` by default.
- BFS does not modify the original maze; SDL2 renders the route through a separate overlay.
- SDL2 initialization, animation, and cleanup are separate; resources are released once when the application exits.
- After a change, rebuild with `make` and run the default maze through a complete animation.

---

## Français

### Présentation

Maze Solver est un projet de résolution et de visualisation animée de labyrinthes, développé en **C avec SDL2**. Le programme charge un labyrinthe depuis un fichier texte, utilise une **recherche en largeur (BFS)** pour déterminer le plus court chemin entre le point de départ de la souris et le fromage, puis anime le déplacement de la souris dans une fenêtre SDL2.

Une implémentation récursive de la **recherche en profondeur (DFS)** est également conservée afin de faciliter l’apprentissage et la comparaison des algorithmes.

### Fonctionnalités

- Chargement et validation stricte du format des labyrinthes texte
- Recherche du plus court chemin dans un labyrinthe non pondéré à quatre directions avec BFS
- Conservation du solveur DFS pour l’apprentissage et la comparaison
- Rendu SDL2 des murs, du chemin, de la souris et du fromage
- Animation du déplacement du départ jusqu’à l’arrivée
- Choix entre rejouer et revenir à l’écran initial après la résolution
- Prise en charge de la fermeture de la fenêtre pendant l’animation
- Écran d’accueil permettant de lancer la résolution, créer un labyrinthe, choisir la langue ou quitter
- Interface intégrée en chinois, anglais et français, sans dépendance aux polices du système
- Fenêtre librement redimensionnable avec conservation des proportions et des zones cliquables
- Génération aléatoire de labyrinthes parfaits en C

### Structure du projet

| Chemin | Rôle |
| --- | --- |
| `src/main.c` | Point d’entrée : chargement du labyrinthe, appel de BFS et lancement de la visualisation |
| `src/app.c` | Fenêtre SDL2, moteur de rendu, textures et cycle de vie des ressources de l’application |
| `src/maze.c` | Lecture du fichier, validation du format et recherche des marqueurs |
| `src/generator.c` | Générateur aléatoire de labyrinthes parfaits en C |
| `src/menu.c` | Écran d’accueil, sélection de la langue, rendu des boutons et interactions |
| `src/ui.c` | Textes chinois, anglais et français et rendu de la police bitmap UTF-8 |
| `src/solver.c` | Solveurs DFS et BFS et stockage du chemin |
| `src/visualize.c` | Rendu du labyrinthe et animation du chemin |
| `include/` | En-têtes publics des modules |
| `assets/maze.txt` | Labyrinthe par défaut |
| `assets/mouse.bmp` | Image de la souris |
| `assets/cheese.bmp` | Image du fromage |
| `assets/ui_font.hex` | Sous-ensemble de glyphes chinois requis par l’interface |
| `tools/maze_gen.c` | Point d’entrée en ligne de commande du générateur C |
| `python/maze_gen.py` | Ancien générateur Python obsolète, conservé comme référence |
| `Makefile` | Règles de compilation Linux/WSL et de compilation croisée Windows |

### Dépendances

- Un compilateur compatible C11, par exemple GCC
- Les fichiers de développement SDL2

Sous Ubuntu, Debian ou WSL :

```bash
sudo apt install build-essential libsdl2-dev
```

### Compilation et exécution

Depuis la racine du projet :

```bash
make
./maze_solver
```

Il est également possible d’utiliser directement GCC :

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic src/*.c -Iinclude \
  $(sdl2-config --cflags --libs) -o maze_solver
./maze_solver
```

L’application utilise le chinois par défaut et affiche un écran d’accueil comportant quatre boutons :

- `开始` : résoudre et animer le labyrinthe actuel
- `新迷宫` : ouvrir l’écran de saisie des dimensions, puis générer le labyrinthe confirmé dans `assets/maze.txt`
- `语言` : ouvrir une page séparée permettant de choisir 中文, English ou Français
- `退出` : quitter l’application

Lorsque la souris atteint le fromage, une fenêtre de fin apparaît au centre du labyrinthe. `REJOUER` relance l’animation du labyrinthe actuel ; `QUITTER` ou `Esc` revient à l’écran initial. Seule la fermeture de la fenêtre SDL quitte entièrement l’application.

Sur l’écran des dimensions, cliquez sur `LARGEUR` ou `HAUTEUR`, puis saisissez des chiffres pour remplacer la valeur. `Tab` change de champ et Retour arrière supprime un chiffre. `Entrée` ou `CRÉER` confirme ; `Esc` ou `ANNULER` revient au menu sans créer de fichier. Les deux dimensions doivent être des nombres impairs compris entre `3` et `99`, et ne peuvent pas être toutes les deux égales à `3`.

La fenêtre peut être librement redimensionnée. SDL2 redimensionne proportionnellement les canevas logiques du menu et du labyrinthe, avec des bandes lorsque leurs proportions diffèrent. Dans le menu, `Entrée`/Espace lance la résolution, `N` crée un labyrinthe, `L` ouvre le choix de la langue et `Q`/`Esc` quitte. Sur l’écran des langues, `C`, `E` et `F` sélectionnent le chinois, l’anglais et le français. Pendant l’animation, `Esc` revient au menu.

### Compilation croisée pour Windows depuis Linux / WSL

Le Makefile fournit une cible MinGW-w64 pour compiler une version **Windows 64 bits**. Installez d’abord le compilateur croisé sous Ubuntu, Debian ou WSL :

```bash
sudo apt install mingw-w64
```

Compilez ensuite l’exécutable Windows depuis la racine du projet :

```bash
make windows
```

Le résultat est créé ici :

```text
dist/maze_solver.exe
```

Pour copier également la DLL SDL2 et le répertoire de ressources `assets/`, exécutez :

```bash
make package-win
```

Copiez ensuite le répertoire `dist/` complet sur la machine Windows et lancez `maze_solver.exe` depuis ce répertoire. Par défaut, le Makefile utilise `x86_64-w64-mingw32-gcc` et le paquet SDL2 fourni dans `lib/x86_64-w64-mingw32`. Si SDL2 se trouve ailleurs, modifiez d’abord `SDL2_WIN`. L’option d’édition de liens `-mwindows` est activée ; la version Windows n’ouvre donc pas de console par défaut.

### Format du fichier de labyrinthe

La première ligne contient :

```text
nombre_de_colonnes nombre_de_lignes
```

Elle doit être suivie exactement du nombre de lignes indiqué, chacune contenant exactement le nombre de caractères déclaré. Seuls les caractères suivants sont autorisés :

| Caractère | Signification |
| --- | --- |
| `#` | Mur infranchissable |
| Espace | Passage libre |
| `S` | Point de départ unique |
| `E` | Point d’arrivée unique |

Exemple de labyrinthe `5 × 3` :

```text
5 3
#####
#S E#
#####
```

La taille maximale prise en charge est de `100 × 100`. Le programme refuse les dimensions ou longueurs de ligne incorrectes, les caractères invalides, les données de labyrinthe supplémentaires ainsi que les marqueurs `S` ou `E` absents ou dupliqués.

### Algorithmes

#### BFS (par défaut)

`solve_maze_bfs()` utilise une file pour explorer le labyrinthe couche par couche. Dans un labyrinthe non pondéré où les déplacements se font dans quatre directions, BFS garantit un plus court chemin.

#### DFS (implémentation conservée)

`solve_maze()` utilise une recherche récursive en profondeur avec retour arrière. Elle peut trouver un chemin valide, mais ne garantit pas qu’il soit le plus court. Cette implémentation est conservée pour l’étude et la comparaison.

Les deux solveurs enregistrent le chemin dans `path_x`, `path_y` et `path_len`, dans l’ordre « arrivée vers départ ». Le module de visualisation parcourt cette séquence en sens inverse afin d’animer le déplacement du départ vers l’arrivée.

### Générer un nouveau labyrinthe

Compilez le générateur C, puis exécutez-le depuis la racine du projet pour créer le labyrinthe par défaut de `91 × 91` :

```bash
make generator
./maze_generator
```

La commande par défaut remplace `assets/maze.txt`. Le générateur ne dépend pas de SDL2 et nécessite uniquement un compilateur C11.

Vous pouvez aussi indiquer la largeur, la hauteur, le fichier de sortie et une graine aléatoire facultative :

```bash
./maze_generator 51 41 assets/maze.txt 12345
```

La largeur et la hauteur doivent être des nombres impairs compris entre `3` et `100` ; la valeur maximale effective est donc `99`. La taille `3 × 3` est refusée, car le départ et l’arrivée se chevaucheraient. Le générateur utilise un DFS aléatoire non récursif et produit un labyrinthe aux limites fermées, dont tous les passages sont connectés et sans cycle, directement dans le format attendu par le lecteur. Les mêmes dimensions et la même graine produisent exactement le même labyrinthe.

Pour exécuter les tests automatisés du générateur :

```bash
make test-generator
```

Pour exécuter sans affichage les tests du menu, des entrées, du cycle de vie SDL et de la mise à l’échelle logique :

```bash
make test-ui
```

L’ancien fichier `python/maze_gen.py` est obsolète et conservé uniquement comme référence. Il ne participe ni à la compilation ni à l’exécution actuelles.

### État actuel du développement et suivi des problèmes

L’application comprend actuellement un cycle de vie SDL séparé, un générateur C de labyrinthes parfaits, un écran d’accueil à quatre boutons, un choix parmi trois langues, une fenêtre de fin et des canevas logiques redimensionnables. Les problèmes d’interface suivants sont conservés comme tests de non-régression :

1. **Corrigé :** `NEW MAZE` ouvre désormais un écran de saisie des dimensions avec sélection des champs à la souris, saisie numérique, Retour arrière, changement de champ avec `Tab`, confirmation ou annulation, et validation des dimensions impaires avant la génération.
2. **Corrigé :** les clics après redimensionnement échouaient parce que les coordonnées de souris déjà transformées par SDL étaient converties une seconde fois. Le menu utilise maintenant directement les coordonnées logiques fournies par SDL, avec un test de non-régression couvrant les clics après un redimensionnement non proportionnel.

### Notes de développement

- Le projet cible C11 et active par défaut `-Wall -Wextra -Wpedantic`.
- BFS ne modifie pas le labyrinthe d’origine ; SDL2 dessine l’animation avec une couche de chemin séparée.
- L’initialisation de SDL2, l’animation et le nettoyage des ressources sont séparés ; les ressources sont libérées une seule fois à la fermeture de l’application.
- Après une modification, recompilez avec `make` et vérifiez une animation complète avec le labyrinthe par défaut.

---

## 日本語

### 概要

Maze Solver は、**C と SDL2** で実装された迷路探索・アニメーション可視化プロジェクトです。テキストファイルから迷路を読み込み、**幅優先探索（BFS）** を使ってネズミの開始地点からチーズの目的地点までの最短経路を求め、その移動を SDL2 ウィンドウ内でアニメーション表示します。

アルゴリズムの学習や比較のため、再帰的な **深さ優先探索（DFS）** の実装も残されています。

### 機能

- テキスト迷路の読み込みと厳密な形式検証
- 上下左右に移動する重みなし迷路の最短経路を BFS で探索
- 学習・比較用の DFS ソルバーを保持
- SDL2 による壁、経路、ネズミ、チーズの描画
- 開始地点から目的地点までの移動アニメーション
- 探索完了後に「もう一度」または初期画面へ戻る操作を選択可能
- アニメーション中もウィンドウを閉じる操作に応答
- 探索開始、新規迷路生成、言語選択、終了を行うスタート画面
- OS のフォントに依存しない中国語・英語・フランス語の内蔵インターフェース
- メニューと迷路の比率およびクリック領域を保った自由なウィンドウサイズ変更
- C によるランダムな完全迷路の生成

### プロジェクト構成

| パス | 役割 |
| --- | --- |
| `src/main.c` | エントリーポイント：迷路の読み込み、BFS の呼び出し、可視化の開始 |
| `src/app.c` | SDL2 ウィンドウ、レンダラー、テクスチャ、アプリケーション全体のリソース管理 |
| `src/maze.c` | 迷路ファイルの読み込み、形式検証、マーカー検索 |
| `src/generator.c` | C 版ランダム完全迷路ジェネレーター |
| `src/menu.c` | スタート画面、言語選択画面、ボタン描画、入力処理 |
| `src/ui.c` | 中国語・英語・フランス語の UI テキストと UTF-8 ビットマップフォント描画 |
| `src/solver.c` | DFS、BFS、経路記録処理 |
| `src/visualize.c` | 迷路描画と経路アニメーション |
| `include/` | 各モジュールの公開ヘッダー |
| `assets/maze.txt` | デフォルト迷路 |
| `assets/mouse.bmp` | ネズミ画像 |
| `assets/cheese.bmp` | チーズ画像 |
| `assets/ui_font.hex` | UI に必要な中国語グリフのサブセット |
| `tools/maze_gen.c` | C 版ジェネレーターのコマンドラインエントリーポイント |
| `python/maze_gen.py` | 参考用に残された非推奨の旧 Python ジェネレーター |
| `Makefile` | Linux/WSL ビルドおよび Windows クロスコンパイル規則 |

### 依存関係

- C11 対応コンパイラー（GCC など）
- SDL2 開発ライブラリ

Ubuntu / Debian / WSL では次を実行します：

```bash
sudo apt install build-essential libsdl2-dev
```

### ビルドと実行

プロジェクトのルートディレクトリで実行します：

```bash
make
./maze_solver
```

GCC を直接使用することもできます：

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic src/*.c -Iinclude \
  $(sdl2-config --cflags --libs) -o maze_solver
./maze_solver
```

アプリケーションはデフォルトで中国語を使用し、4 つのボタンを持つスタート画面を表示します：

- `开始`：現在の迷路を解いてアニメーションを再生
- `新迷宫`：幅と高さの入力画面を開き、確定後に新しい迷路を生成して `assets/maze.txt` に保存
- `语言`：専用の言語選択画面を開き、中文、English、Français から選択
- `退出`：アプリケーションを終了

ネズミがチーズに到達すると、迷路の中央に完了ウィンドウが表示されます。「もう一度」を選ぶと現在の迷路を再生し、「終了」または `Esc` を選ぶと初期画面に戻ります。SDL ウィンドウを閉じた場合のみ、アプリケーション全体が終了します。

サイズ入力画面では、`WIDTH` または `HEIGHT` をクリックして数字を入力すると現在値を置き換えられます。`Tab` で入力欄を切り替え、Backspace で数字を削除します。`Enter` または `GENERATE` で確定し、`Esc` または `CANCEL` でファイルを生成せずにメニューへ戻ります。幅と高さは `3` から `99` までの奇数である必要があり、両方を同時に `3` にすることはできません。

ウィンドウは自由にサイズ変更できます。SDL2 はメニューと迷路の論理キャンバスを縦横比を保って拡大縮小し、比率が異なる場合は余白を追加します。メニューでは `Enter`/Space で開始、`N` で迷路生成、`L` で言語選択画面を開き、`Q`/`Esc` で終了します。言語画面では `C`、`E`、`F` で中国語、英語、フランス語を選択できます。アニメーション中に `Esc` を押すとメニューへ戻ります。

### Linux / WSL から Windows 版をクロスコンパイル

Makefile には **64 ビット Windows** 向けの MinGW-w64 クロスコンパイルターゲットがあります。まず Ubuntu、Debian、または WSL にクロスコンパイラーをインストールします：

```bash
sudo apt install mingw-w64
```

次にプロジェクトのルートディレクトリで Windows 実行ファイルをビルドします：

```bash
make windows
```

生成先：

```text
dist/maze_solver.exe
```

SDL2 ランタイム DLL と `assets/` リソースディレクトリもコピーする場合は、次を実行します：

```bash
make package-win
```

完了後、`dist/` ディレクトリ全体を Windows マシンへコピーし、そのディレクトリから `maze_solver.exe` を起動します。Makefile はデフォルトで `x86_64-w64-mingw32-gcc` と、プロジェクト内の `lib/x86_64-w64-mingw32` にある SDL2 開発パッケージを使用します。SDL2 の場所が異なる場合は、先に `SDL2_WIN` を変更してください。現在のリンクオプションには `-mwindows` が含まれるため、Windows 版では通常コンソールウィンドウが表示されません。

### 迷路ファイル形式

迷路ファイルの先頭行は次の形式です：

```text
列数 行数
```

その後に指定された行数分の迷路データが続きます。各行は指定された列数と正確に一致し、使用できる文字は次のものだけです：

| 文字 | 意味 |
| --- | --- |
| `#` | 壁（通行不可） |
| 空白 | 通路 |
| `S` | 一意の開始地点 |
| `E` | 一意の目的地点 |

`5 × 3` の例：

```text
5 3
#####
#S E#
#####
```

対応する最大サイズは `100 × 100` です。寸法や行の長さが不正な場合、無効な文字や余分な迷路データがある場合、または `S` と `E` が存在しない、あるいは重複している場合は拒否されます。

### アルゴリズム

#### BFS（デフォルト）

`solve_maze_bfs()` はキューを使用して距離ごとの層を順番に探索します。本プロジェクトのように各移動のコストが同じで上下左右にのみ移動できる迷路では、BFS は最短経路を保証します。

#### DFS（保持されている実装）

`solve_maze()` は再帰的な深さ優先探索とバックトラッキングを使用します。有効な経路を見つけられますが、最短であることは保証されません。この実装はアルゴリズムの比較と学習のために残されています。

どちらのソルバーも経路を `path_x`、`path_y`、`path_len` に「目的地点から開始地点」の順で記録します。可視化モジュールはこの並びを逆順に読み取り、開始地点から目的地点までの移動をアニメーション表示します。

### 新しい迷路の生成

C 版ジェネレーターをビルドし、プロジェクトのルートディレクトリから実行して、デフォルトの `91 × 91` 迷路を生成します：

```bash
make generator
./maze_generator
```

デフォルトのコマンドは `assets/maze.txt` を上書きします。ジェネレーター自体は SDL2 に依存せず、C11 コンパイラーだけを必要とします。

幅、高さ、出力ファイル、および任意の乱数シードを指定することもできます：

```bash
./maze_generator 51 41 assets/maze.txt 12345
```

幅と高さは `3` から `100` の範囲にある奇数でなければならないため、実際の最大値は `99` です。`3 × 3` は開始地点と目的地点が重なるため拒否されます。ジェネレーターは非再帰のランダム DFS を使用し、境界が閉じ、すべての通路が連結かつ閉路を持たない迷路を、ローダーが要求する形式で直接出力します。同じ寸法と同じシードを指定すると、まったく同じ迷路を再現できます。

ジェネレーターの自動テストを実行します：

```bash
make test-generator
```

メニュー、入力、SDL ライフサイクル、論理スケーリングのヘッドレステストを実行します：

```bash
make test-ui
```

旧 `python/maze_gen.py` は非推奨で、参考用としてのみ残されています。現在のビルドや実行には使用されません。

### 現在の開発状況と問題記録

現在のアプリケーションには、分離された SDL ライフサイクル、C 版完全迷路ジェネレーター、4 ボタンのスタートメニュー、3 言語の選択、完了ダイアログ、サイズ変更可能な論理キャンバスが含まれています。以下の UI 問題は回帰テストの記録として残されています：

1. **修正済み：** `NEW MAZE` は寸法入力画面を開き、マウスによる入力欄の選択、数字入力、Backspace、`Tab` による切り替え、確定とキャンセル、および生成前の奇数範囲検証に対応しています。
2. **修正済み：** サイズ変更後にボタンをクリックできなかった原因は、SDL がすでに変換したマウス座標を再変換していたことでした。現在は SDL が提供する論理イベント座標を直接使用し、非等比例リサイズ後のクリックを確認する回帰テストを追加しています。

### 開発上の注意

- プロジェクトは C11 を対象とし、デフォルトで `-Wall -Wextra -Wpedantic` を有効にしています。
- BFS は元の迷路を変更しません。SDL2 は独立した経路オーバーレイを使ってアニメーションを描画します。
- SDL2 の初期化、アニメーション、リソース解放は分離され、アプリケーション終了時に一度だけリソースを解放します。
- 変更後は `make` で再ビルドし、デフォルト迷路でアニメーション全体を確認してください。
