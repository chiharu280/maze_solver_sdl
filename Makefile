# === 通用部分（你原来就有）===
CC      ?= gcc
CFLAGS  ?= -std=c11 -Wall -Wextra -Wpedantic
SRC_DIR := src
INC_DIR := include
SRCS    := $(wildcard $(SRC_DIR)/*.c)
LINUX_TARGET := maze_solver
GENERATOR_TARGET := maze_generator
GENERATOR_SRCS := tools/maze_gen.c src/generator.c
GENERATOR_TEST_TARGET := build/test_generator
GENERATOR_TEST_SRCS := tests/test_generator.c src/generator.c src/maze.c src/solver.c
MENU_TEST_TARGET := build/test_menu
MENU_TEST_SRCS := tests/test_menu.c src/app.c src/menu.c src/maze.c src/ui.c src/visualize.c

SDL_FLAGS := `sdl2-config --cflags --libs`

all: $(LINUX_TARGET)

.PHONY: all clean generator test-generator test-ui windows package-win

$(LINUX_TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -I$(INC_DIR) -o $@ $(SDL_FLAGS)

generator: $(GENERATOR_TARGET)

$(GENERATOR_TARGET): $(GENERATOR_SRCS) include/generator.h include/maze.h
	$(CC) $(CFLAGS) $(GENERATOR_SRCS) -I$(INC_DIR) -o $@

test-generator: $(GENERATOR_TEST_TARGET)
	./$(GENERATOR_TEST_TARGET)

$(GENERATOR_TEST_TARGET): $(GENERATOR_TEST_SRCS) include/generator.h include/maze.h include/solver.h
	mkdir -p build
	$(CC) $(CFLAGS) $(GENERATOR_TEST_SRCS) -I$(INC_DIR) -o $@

test-ui: $(MENU_TEST_TARGET)
	SDL_VIDEODRIVER=dummy ./$(MENU_TEST_TARGET)

$(MENU_TEST_TARGET): $(MENU_TEST_SRCS) include/app.h include/menu.h include/ui.h include/visualize.h
	mkdir -p build
	$(CC) $(CFLAGS) $(MENU_TEST_SRCS) -I$(INC_DIR) -o $@ $(SDL_FLAGS)

clean:
	rm -f $(LINUX_TARGET) $(GENERATOR_TARGET) build/*.o \
		$(GENERATOR_TEST_TARGET) $(MENU_TEST_TARGET) $(WIN_TARGET) dist/*.dll

# === 下面是新增的 Windows 交叉编译部分 ===

# 选择 64 位 Windows，可改成 i686-w64-mingw32 做 32 位
CROSS      := x86_64-w64-mingw32
CC_WIN     := $(CROSS)-gcc

# 你放 SDL2 Windows 开发包的位置（按你的实际路径改）
SDL2_WIN   := lib/x86_64-w64-mingw32

CFLAGS_WIN := -I$(INC_DIR) -I$(SDL2_WIN)/include
LDFLAGS_WIN := -L$(SDL2_WIN)/lib -lmingw32 -lSDL2main -lSDL2
# 如果不需要隐藏控制台窗口，可以去掉 -mwindows
LDFLAGS_WIN += -mwindows

WIN_TARGET := dist/maze_solver.exe

windows: $(WIN_TARGET)

$(WIN_TARGET): $(SRCS)
	mkdir -p dist
	$(CC_WIN) $(CFLAGS) $(SRCS) $(CFLAGS_WIN) -o $(WIN_TARGET) $(LDFLAGS_WIN)

# 方便把运行时 DLL 与资源拷到发行目录
package-win: windows
	# SDL2 运行时 DLL（按你的包里实际路径改）
	cp $(SDL2_WIN)/bin/SDL2.dll dist/
	# 你的资源
	cp -r assets dist/
	# 如果程序需要在启动目录读取 maze.txt 等，也一并拷过去
	# cp assets/maze.txt dist/
