#include <stdint.h>
#include <stdio.h>

#include "generator.h"

typedef struct {
    int x;
    int y;
} GeneratorPoint;

static const int DIRECTIONS[4][2] = {
    {0, 1}, {1, 0}, {0, -1}, {-1, 0}
};

/* A small self-contained PRNG avoids changing the process-wide rand() state. */
static uint32_t next_random(uint32_t* state) {
    uint32_t value = *state;

    if (value == 0) {
        value = UINT32_C(0x6d2b79f5);
    }
    value ^= value << 13;
    value ^= value >> 17;
    value ^= value << 5;
    *state = value;
    return value;
}

static int dimensions_are_valid(int width, int height) {
    if (width < 3 || height < 3 || width > MAX_COLS || height > MAX_ROWS) {
        return 0;
    }
    if (width % 2 == 0 || height % 2 == 0) {
        return 0;
    }

    /* In a 3 x 3 maze, S and E would occupy the same cell. */
    return width != 3 || height != 3;
}

int generate_perfect_maze(char output[][MAX_COLS + 1], int width, int height,
                          uint32_t seed) {
    GeneratorPoint stack[MAX_ROWS * MAX_COLS];
    int stack_size = 0;
    uint32_t random_state = seed;

    if (!output || !dimensions_are_valid(width, height)) {
        return 0;
    }

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            output[y][x] = '#';
        }
        output[y][width] = '\0';
    }

    output[1][1] = ' ';
    stack[stack_size++] = (GeneratorPoint){1, 1};

    while (stack_size > 0) {
        GeneratorPoint current = stack[stack_size - 1];
        int candidates[4];
        int candidate_count = 0;

        for (int direction = 0; direction < 4; ++direction) {
            int next_x = current.x + DIRECTIONS[direction][0] * 2;
            int next_y = current.y + DIRECTIONS[direction][1] * 2;

            if (next_x > 0 && next_x < width - 1 &&
                next_y > 0 && next_y < height - 1 &&
                output[next_y][next_x] == '#') {
                candidates[candidate_count++] = direction;
            }
        }

        if (candidate_count == 0) {
            --stack_size;
            continue;
        }

        int direction = candidates[next_random(&random_state) %
                                   (uint32_t)candidate_count];
        int step_x = DIRECTIONS[direction][0];
        int step_y = DIRECTIONS[direction][1];
        int next_x = current.x + step_x * 2;
        int next_y = current.y + step_y * 2;

        output[current.y + step_y][current.x + step_x] = ' ';
        output[next_y][next_x] = ' ';
        stack[stack_size++] = (GeneratorPoint){next_x, next_y};
    }

    output[1][1] = 'S';
    output[height - 2][width - 2] = 'E';
    return 1;
}

int generate_maze_file(const char* filename, int width, int height,
                       uint32_t seed) {
    char generated[MAX_ROWS][MAX_COLS + 1];

    if (!filename || !generate_perfect_maze(generated, width, height, seed)) {
        return 0;
    }

    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("无法创建迷宫文件");
        return 0;
    }

    int success = fprintf(file, "%d %d\n", width, height) >= 0;
    for (int y = 0; success && y < height; ++y) {
        success = fwrite(generated[y], 1, (size_t)width, file) ==
                      (size_t)width &&
                  fputc('\n', file) != EOF;
    }

    if (fclose(file) != 0) {
        success = 0;
    }
    if (!success) {
        fprintf(stderr, "写入迷宫文件失败: %s\n", filename);
    }
    return success;
}
