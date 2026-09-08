#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "generator.h"

#define DEFAULT_WIDTH 91
#define DEFAULT_HEIGHT 91
#define DEFAULT_OUTPUT "assets/maze.txt"

static int parse_int(const char* text, int* value) {
    char* end = NULL;
    errno = 0;
    long parsed = strtol(text, &end, 10);

    if (errno != 0 || end == text || *end != '\0' || parsed < 1 ||
        parsed > 100) {
        return 0;
    }
    *value = (int)parsed;
    return 1;
}

static int parse_seed(const char* text, uint32_t* seed) {
    char* end = NULL;
    errno = 0;
    if (text[0] == '-') {
        return 0;
    }
    unsigned long parsed = strtoul(text, &end, 10);

    if (errno != 0 || end == text || *end != '\0' || parsed > UINT32_MAX) {
        return 0;
    }
    *seed = (uint32_t)parsed;
    return 1;
}

static void print_usage(const char* program) {
    fprintf(stderr,
            "用法: %s [宽度 高度 [输出文件 [随机种子]]]\n"
            "宽度和高度必须是不超过 100 的奇数，默认生成 "
            "91 x 91 到 assets/maze.txt。\n",
            program);
}

int main(int argc, char* argv[]) {
    int width = DEFAULT_WIDTH;
    int height = DEFAULT_HEIGHT;
    const char* output = DEFAULT_OUTPUT;
    uint32_t seed = (uint32_t)time(NULL) ^ (uint32_t)clock();

    if (argc != 1 && argc != 3 && argc != 4 && argc != 5) {
        print_usage(argv[0]);
        return 1;
    }
    if (argc >= 3 &&
        (!parse_int(argv[1], &width) || !parse_int(argv[2], &height))) {
        print_usage(argv[0]);
        return 1;
    }
    if (argc >= 4) {
        output = argv[3];
    }
    if (argc == 5 && !parse_seed(argv[4], &seed)) {
        print_usage(argv[0]);
        return 1;
    }

    if (!generate_maze_file(output, width, height, seed)) {
        fprintf(stderr, "生成迷宫失败，请检查尺寸和输出路径。\n");
        return 1;
    }

    printf("迷宫已保存到 %s（大小：%d x %d，种子：%u）\n",
           output, width, height, (unsigned int)seed);
    return 0;
}
