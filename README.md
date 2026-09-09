# Maze Solver 2.0

[English](#english) | [中文](#中文) | [Français](#français) | [日本語](#日本語)

---

## 中文

一个基于 **C11** 与 **SDL2** 开发的跨平台迷宫求解与动画可视化系统，支持路径动画、算法对比（BFS/DFS）以及随机完美迷宫生成。

### 🌟 核心特性

- **高效求解与对比**：默认使用 **BFS** 求解四方向无权迷宫的最短路径，保留递归 **DFS** 求解器用于算法学习与对比。
- **路径可视化**：基于 SDL2 逐步播放求解路径，支持 `1`–`5` 档动画速度调节。
- **随机生成**：内置 C 语言编写的非递归 DFS 完美迷宫生成器，保证通路连通无环且边界封闭。
- **轻量与跨平台**：除 SDL2 外不引入额外运行时库，内置 UTF-8 位图字体；支持 Linux/WSL 原生构建与 Windows（MinGW-w64）交叉编译。
- **多语言与自适应**：支持中文、英文、法文和日文四种界面语言，窗口可自由缩放并保持正确比例和点击区域。

### 🚀 快速上手

#### 1. 环境依赖

- **编译器**：GCC 或 Clang（支持 C11 标准）
- **开发库**：SDL2

Ubuntu / Debian / WSL：

```bash
sudo apt install build-essential libsdl2-dev
```

#### 2. 编译与运行

在项目根目录下执行：

```bash
make
./maze_solver
```

也可以直接使用 GCC 编译：

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic src/*.c -Iinclude \
  $(sdl2-config --cflags --libs) -o maze_solver
./maze_solver
```

### 🎮 控制与操作

| 界面/场景 | 键盘快捷键 | 鼠标/交互操作 |
| --- | --- | --- |
| **主菜单** | `Enter`/空格（开始）、`N`（新迷宫）、`S`（设置）、`L`（语言）、`Q`/`Esc`（退出） | 点击对应按钮 |
| **语言选择** | `C`（中文）、`E`（English）、`F`（Français）、`J`（日本語） | 点击选择语言 |
| **速度设置** | 方向键 `←`/`→` 或数字键 `1`–`5` | 点击或拖动滑块（`1` 最慢，`5` 最快） |
| **尺寸输入** | `Tab` 切换输入框、`0`–`9` 输入、退格删除、`Enter` 确认、`Esc` 取消 | 点击输入框或确认/取消按钮；尺寸须为 `3`–`99` 的奇数，且不能同时为 `3` |
| **动画播放** | `Esc` 中断播放并返回菜单 | 播放完毕后可选择“再来一次”或返回菜单 |

### 🎲 迷宫生成器（CLI）

除了在 GUI 菜单中直接生成，也可以独立编译和调用命令行生成器：

```bash
# 编译并使用默认参数生成 assets/maze.txt（91 × 91）
make generator
./maze_generator

# 自定义参数：./maze_generator <宽度> <高度> [输出路径 [随机种子]]
./maze_generator 51 41 assets/maze.txt 12345
```

### 🛠️ 项目结构

```text
├── assets/             # 贴图、默认迷宫及 ui_font.hex 位图字体
├── include/            # C 头文件
├── src/
│   ├── main.c          # 程序入口与主逻辑
│   ├── solver.c        # BFS 与 DFS 核心算法实现
│   ├── generator.c     # 完美迷宫生成算法
│   ├── visualize.c     # 路径动画渲染
│   ├── menu.c / ui.c   # 界面、按钮与多语言位图字体渲染
│   └── app.c / maze.c  # SDL2 资源生命周期管理与文件解析
├── tools/              # 命令行生成器入口
├── Makefile            # 构建脚本（含交叉编译与测试目标）
└── README.md
```

### 📄 迷宫文件格式

程序从文本文件加载迷宫，首行声明尺寸，后续为网格数据：

```text
5 3
#####
#S E#
#####
```

- **格式规范**：第一行为 `列数 行数`，最大支持 `100 × 100`。
- **字符定义**：`#` 为墙体，空格为通路，`S` 为唯一的起点，`E` 为唯一的终点。

### 📦 交叉编译（Windows）

在 Linux / WSL 环境下可以直接打包 Windows 发行目录：

```bash
# 1. 安装交叉编译器
sudo apt install mingw-w64

# 2. 构建并打包 Windows 发行目录（含 DLL 与资源）
make package-win
```

产物位于 `dist/`，将整个目录复制到 Windows 环境后即可运行 `maze_solver.exe`。


---

## English

A cross-platform maze-solving and animation system built with **C11** and **SDL2**, featuring path animation, BFS/DFS comparison, and random perfect-maze generation.

### 🌟 Core features

- **Efficient solving and comparison:** Uses **BFS** by default to find a shortest path in a four-direction, unweighted maze, while retaining a recursive **DFS** solver for learning and comparison.
- **Path visualization:** Animates the solved path step by step with SDL2 and provides adjustable speed levels from `1` to `5`.
- **Random generation:** Includes an iterative DFS perfect-maze generator written in C, producing a connected, acyclic maze with closed boundaries.
- **Lightweight and cross-platform:** Requires no additional runtime library beyond SDL2 and uses a built-in UTF-8 bitmap font; supports native Linux/WSL builds and Windows cross-compilation with MinGW-w64.
- **Multilingual and responsive:** Provides Chinese, English, French, and Japanese interfaces in a freely resizable window with correctly scaled visuals and hit targets.

### 🚀 Quick start

#### 1. Requirements

- **Compiler:** GCC or Clang with C11 support
- **Development library:** SDL2

Ubuntu / Debian / WSL:

```bash
sudo apt install build-essential libsdl2-dev
```

#### 2. Build and run

From the project root:

```bash
make
./maze_solver
```

You can also compile directly with GCC:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic src/*.c -Iinclude \
  $(sdl2-config --cflags --libs) -o maze_solver
./maze_solver
```

### 🎮 Controls

| Screen/context | Keyboard shortcuts | Mouse/interaction |
| --- | --- | --- |
| **Main menu** | `Enter`/Space (start), `N` (new maze), `S` (settings), `L` (language), `Q`/`Esc` (quit) | Click the corresponding button |
| **Language selection** | `C` (Chinese), `E` (English), `F` (French), `J` (Japanese) | Click a language |
| **Speed settings** | `←`/`→` or number keys `1`–`5` | Click or drag the slider (`1` is slowest, `5` is fastest) |
| **Size input** | `Tab` to switch fields, `0`–`9` to type, Backspace to delete, `Enter` to confirm, `Esc` to cancel | Click a field or the confirm/cancel buttons; dimensions must be odd values from `3` to `99` and cannot both be `3` |
| **Animation** | `Esc` stops playback and returns to the menu | When playback finishes, replay the maze or return to the menu |

### 🎲 Maze generator (CLI)

In addition to generating a maze from the GUI, you can build and run the standalone command-line generator:

```bash
# Build and generate the default assets/maze.txt (91 × 91)
make generator
./maze_generator

# Custom arguments: ./maze_generator <width> <height> [output-path [seed]]
./maze_generator 51 41 assets/maze.txt 12345
```

### 🛠️ Project layout

```text
├── assets/             # Sprites, default maze, and ui_font.hex bitmap font
├── include/            # C header files
├── src/
│   ├── main.c          # Entry point and application flow
│   ├── solver.c        # Core BFS and DFS implementations
│   ├── generator.c     # Perfect-maze generation algorithm
│   ├── visualize.c     # Path-animation rendering
│   ├── menu.c / ui.c   # Screens, buttons, localization, and bitmap font
│   └── app.c / maze.c  # SDL2 resource lifecycle and maze-file parsing
├── tools/              # Command-line generator entry point
├── Makefile            # Build rules, cross-build, and test targets
└── README.md
```

### 📄 Maze file format

The program loads a text maze whose first line declares its dimensions, followed by the grid:

```text
5 3
#####
#S E#
#####
```

- **Format:** The first line is `columns rows`; the maximum supported size is `100 × 100`.
- **Cells:** `#` is a wall, Space is a passage, `S` is the unique start, and `E` is the unique destination.

### 📦 Cross-compile for Windows

On Linux / WSL, you can build a complete Windows distribution directory:

```bash
# 1. Install the cross-compiler
sudo apt install mingw-w64

# 2. Build and package the Windows distribution (including the DLL and assets)
make package-win
```

The output is placed in `dist/`. Copy the entire directory to Windows and run `maze_solver.exe`.


---

## Français

Un système multiplateforme de résolution et de visualisation animée de labyrinthes, développé en **C11** avec **SDL2**, proposant l’animation des chemins, la comparaison BFS/DFS et la génération aléatoire de labyrinthes parfaits.

### 🌟 Fonctionnalités principales

- **Résolution efficace et comparaison :** utilise **BFS** par défaut pour trouver un plus court chemin dans un labyrinthe non pondéré à quatre directions, tout en conservant un solveur **DFS** récursif pour l’apprentissage et la comparaison.
- **Visualisation du chemin :** anime pas à pas le chemin résolu avec SDL2 et propose cinq niveaux de vitesse réglables, de `1` à `5`.
- **Génération aléatoire :** comprend un générateur itératif de labyrinthes parfaits fondé sur DFS et écrit en C ; les labyrinthes produits sont connexes, acycliques et entourés d’une bordure fermée.
- **Léger et multiplateforme :** ne requiert aucune bibliothèque d’exécution supplémentaire au-delà de SDL2 et utilise une police bitmap UTF-8 intégrée ; prend en charge la compilation native sous Linux/WSL et la compilation croisée Windows avec MinGW-w64.
- **Multilingue et adaptatif :** propose des interfaces en chinois, anglais, français et japonais dans une fenêtre librement redimensionnable, avec un rendu et des zones cliquables correctement mis à l’échelle.

### 🚀 Démarrage rapide

#### 1. Prérequis

- **Compilateur :** GCC ou Clang avec prise en charge de C11
- **Bibliothèque de développement :** SDL2

Ubuntu / Debian / WSL :

```bash
sudo apt install build-essential libsdl2-dev
```

#### 2. Compilation et exécution

Depuis la racine du projet :

```bash
make
./maze_solver
```

Vous pouvez également compiler directement avec GCC :

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic src/*.c -Iinclude \
  $(sdl2-config --cflags --libs) -o maze_solver
./maze_solver
```

### 🎮 Commandes

| Écran/contexte | Raccourcis clavier | Souris/interaction |
| --- | --- | --- |
| **Menu principal** | `Entrée`/Espace (démarrer), `N` (nouveau labyrinthe), `S` (réglages), `L` (langue), `Q`/`Échap` (quitter) | Cliquer sur le bouton correspondant |
| **Sélection de la langue** | `C` (chinois), `E` (anglais), `F` (français), `J` (japonais) | Cliquer sur une langue |
| **Réglage de la vitesse** | `←`/`→` ou touches numériques `1`–`5` | Cliquer sur le curseur ou le faire glisser (`1` est le plus lent, `5` le plus rapide) |
| **Saisie des dimensions** | `Tab` pour changer de champ, `0`–`9` pour saisir, Retour arrière pour effacer, `Entrée` pour confirmer, `Échap` pour annuler | Cliquer sur un champ ou sur les boutons de confirmation/annulation ; les dimensions doivent être impaires, comprises entre `3` et `99`, et ne peuvent pas valoir toutes les deux `3` |
| **Animation** | `Échap` interrompt la lecture et revient au menu | À la fin, rejouer le labyrinthe ou revenir au menu |

### 🎲 Générateur de labyrinthes (CLI)

En plus de la génération depuis l’interface graphique, vous pouvez compiler et exécuter le générateur autonome en ligne de commande :

```bash
# Compiler et générer le fichier assets/maze.txt par défaut (91 × 91)
make generator
./maze_generator

# Arguments personnalisés : ./maze_generator <largeur> <hauteur> [chemin-sortie [graine]]
./maze_generator 51 41 assets/maze.txt 12345
```

### 🛠️ Structure du projet

```text
├── assets/             # Images, labyrinthe par défaut et police bitmap ui_font.hex
├── include/            # Fichiers d’en-tête C
├── src/
│   ├── main.c          # Point d’entrée et déroulement de l’application
│   ├── solver.c        # Implémentations principales de BFS et DFS
│   ├── generator.c     # Algorithme de génération de labyrinthes parfaits
│   ├── visualize.c     # Rendu de l’animation du chemin
│   ├── menu.c / ui.c   # Écrans, boutons, traduction et police bitmap
│   └── app.c / maze.c  # Cycle de vie des ressources SDL2 et lecture des labyrinthes
├── tools/              # Point d’entrée du générateur en ligne de commande
├── Makefile            # Règles de compilation, compilation croisée et tests
└── README.md
```

### 📄 Format du fichier de labyrinthe

Le programme charge un labyrinthe texte dont la première ligne indique les dimensions, suivie de la grille :

```text
5 3
#####
#S E#
#####
```

- **Format :** la première ligne est `colonnes lignes` ; la taille maximale prise en charge est de `100 × 100`.
- **Cellules :** `#` représente un mur, l’espace un passage, `S` l’unique départ et `E` l’unique destination.

### 📦 Compilation croisée pour Windows

Sous Linux / WSL, vous pouvez créer un répertoire de distribution Windows complet :

```bash
# 1. Installer le compilateur croisé
sudo apt install mingw-w64

# 2. Compiler et préparer la distribution Windows (avec la DLL et les ressources)
make package-win
```

Le résultat se trouve dans `dist/`. Copiez le répertoire entier sous Windows, puis lancez `maze_solver.exe`.


---

## 日本語

**C11** と **SDL2** で開発されたクロスプラットフォームの迷路探索・アニメーション可視化システムです。経路アニメーション、BFS/DFS の比較、ランダムな完全迷路の生成に対応しています。

### 🌟 主な機能

- **効率的な探索と比較：** 上下左右に移動する重みなし迷路の最短経路をデフォルトで **BFS** により求め、学習・比較用として再帰的な **DFS** ソルバーも保持しています。
- **経路の可視化：** SDL2 で求解経路を段階的に再生し、`1`～`5` のアニメーション速度を設定できます。
- **ランダム生成：** C で実装された非再帰 DFS の完全迷路ジェネレーターを備え、通路が連結かつ非循環で、境界が閉じた迷路を生成します。
- **軽量かつクロスプラットフォーム：** SDL2 以外の追加ランタイムライブラリを必要とせず、UTF-8 ビットマップフォントを内蔵しています。Linux/WSL のネイティブビルドと MinGW-w64 による Windows クロスコンパイルに対応しています。
- **多言語と可変レイアウト：** 中国語、英語、フランス語、日本語の UI に対応し、ウィンドウを自由にサイズ変更しても表示比率とクリック領域が正しく保たれます。

### 🚀 クイックスタート

#### 1. 必要な環境

- **コンパイラー：** C11 に対応する GCC または Clang
- **開発ライブラリ：** SDL2

Ubuntu / Debian / WSL：

```bash
sudo apt install build-essential libsdl2-dev
```

#### 2. ビルドと実行

プロジェクトのルートディレクトリで実行します：

```bash
make
./maze_solver
```

GCC で直接コンパイルすることもできます：

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic src/*.c -Iinclude \
  $(sdl2-config --cflags --libs) -o maze_solver
./maze_solver
```

### 🎮 操作方法

| 画面/状況 | キーボードショートカット | マウス/操作 |
| --- | --- | --- |
| **メインメニュー** | `Enter`/Space（開始）、`N`（新規迷路）、`S`（設定）、`L`（言語）、`Q`/`Esc`（終了） | 対応するボタンをクリック |
| **言語選択** | `C`（中国語）、`E`（英語）、`F`（フランス語）、`J`（日本語） | 言語をクリック |
| **速度設定** | `←`/`→` または数字キー `1`～`5` | スライダーをクリックまたはドラッグ（`1` が最も遅く、`5` が最も速い） |
| **サイズ入力** | `Tab` で入力欄を切り替え、`0`～`9` で入力、Backspace で削除、`Enter` で確定、`Esc` でキャンセル | 入力欄または確定/キャンセルボタンをクリック。寸法は `3`～`99` の奇数で、幅と高さの両方を `3` にはできません |
| **アニメーション** | `Esc` で再生を中断してメニューへ戻る | 再生終了後、もう一度再生するかメニューへ戻るかを選択 |

### 🎲 迷路ジェネレーター（CLI）

GUI メニューからの生成に加えて、単独のコマンドラインジェネレーターもビルドして実行できます：

```bash
# ビルドし、デフォルトの assets/maze.txt（91 × 91）を生成
make generator
./maze_generator

# カスタム引数：./maze_generator <幅> <高さ> [出力先 [乱数シード]]
./maze_generator 51 41 assets/maze.txt 12345
```

### 🛠️ プロジェクト構成

```text
├── assets/             # 画像、デフォルト迷路、ui_font.hex ビットマップフォント
├── include/            # C ヘッダーファイル
├── src/
│   ├── main.c          # エントリーポイントとアプリケーション処理
│   ├── solver.c        # BFS と DFS の主要実装
│   ├── generator.c     # 完全迷路生成アルゴリズム
│   ├── visualize.c     # 経路アニメーションの描画
│   ├── menu.c / ui.c   # 画面、ボタン、多言語化、ビットマップフォント
│   └── app.c / maze.c  # SDL2 リソースのライフサイクルと迷路ファイル解析
├── tools/              # コマンドラインジェネレーターのエントリーポイント
├── Makefile            # ビルド、クロスコンパイル、テストの規則
└── README.md
```

### 📄 迷路ファイル形式

迷路はテキストファイルから読み込まれ、先頭行に寸法、その後にグリッドを記述します：

```text
5 3
#####
#S E#
#####
```

- **形式：** 先頭行は `列数 行数` で、最大サイズは `100 × 100` です。
- **セル：** `#` は壁、空白は通路、`S` は一意の開始地点、`E` は一意の目的地点です。

### 📦 Windows 向けクロスコンパイル

Linux / WSL 上で Windows 用の配布ディレクトリを作成できます：

```bash
# 1. クロスコンパイラーをインストール
sudo apt install mingw-w64

# 2. Windows 配布版をビルドしてパッケージ化（DLL とリソースを含む）
make package-win
```

成果物は `dist/` に生成されます。ディレクトリ全体を Windows 環境へコピーし、`maze_solver.exe` を実行してください。

