#include <stdio.h>
#include <string.h>

#include "generator.h"
#include "maze.h"
#include "solver.h"

#define TEST_FILE "/tmp/maze_solver_generator_test.txt"

typedef struct {
    int x;
    int y;
} TestPoint;

static int failures = 0;

#define CHECK(condition, message)                  \
    do {                                           \
        if (!(condition)) {                        \
            fprintf(stderr, "FAIL: %s\n", message); \
            ++failures;                            \
        }                                          \
    } while (0)

static int is_open(char cell) {
    return cell == ' ' || cell == 'S' || cell == 'E';
}

static void check_perfect_maze(char generated[][MAX_COLS + 1],
                               int width, int height) {
    int visited[MAX_ROWS][MAX_COLS] = {{0}};
    TestPoint queue[MAX_ROWS * MAX_COLS];
    int front = 0;
    int rear = 0;
    int open_cells = 0;
    int edges = 0;
    int start_count = 0;
    int end_count = 0;

    for (int y = 0; y < height; ++y) {
        CHECK(generated[y][width] == '\0', "每行必须以空字符结尾");
        for (int x = 0; x < width; ++x) {
            char cell = generated[y][x];
            CHECK(cell == '#' || is_open(cell), "生成了非法字符");
            if (cell == 'S') {
                ++start_count;
            } else if (cell == 'E') {
                ++end_count;
            }
            if (!is_open(cell)) {
                continue;
            }

            ++open_cells;
            if (x + 1 < width && is_open(generated[y][x + 1])) {
                ++edges;
            }
            if (y + 1 < height && is_open(generated[y + 1][x])) {
                ++edges;
            }
        }
    }

    for (int x = 0; x < width; ++x) {
        CHECK(generated[0][x] == '#' && generated[height - 1][x] == '#',
              "上下边界必须封闭");
    }
    for (int y = 0; y < height; ++y) {
        CHECK(generated[y][0] == '#' && generated[y][width - 1] == '#',
              "左右边界必须封闭");
    }
    CHECK(start_count == 1, "必须恰好生成一个 S");
    CHECK(end_count == 1, "必须恰好生成一个 E");

    queue[rear++] = (TestPoint){1, 1};
    visited[1][1] = 1;
    while (front < rear) {
        static const int directions[4][2] = {
            {1, 0}, {-1, 0}, {0, 1}, {0, -1}
        };
        TestPoint current = queue[front++];

        for (int i = 0; i < 4; ++i) {
            int next_x = current.x + directions[i][0];
            int next_y = current.y + directions[i][1];
            if (next_x >= 0 && next_x < width && next_y >= 0 &&
                next_y < height && !visited[next_y][next_x] &&
                is_open(generated[next_y][next_x])) {
                visited[next_y][next_x] = 1;
                queue[rear++] = (TestPoint){next_x, next_y};
            }
        }
    }

    CHECK(rear == open_cells, "所有通路必须连通");
    CHECK(edges == open_cells - 1, "完美迷宫的通路图必须无环");
}

static void test_dimensions(void) {
    char generated[MAX_ROWS][MAX_COLS + 1] = {{0}};

    CHECK(!generate_perfect_maze(NULL, 91, 91, 1), "必须拒绝空输出缓冲区");
    CHECK(!generate_perfect_maze(generated, 2, 5, 1), "必须拒绝过小尺寸");
    CHECK(!generate_perfect_maze(generated, 4, 5, 1), "必须拒绝偶数宽度");
    CHECK(!generate_perfect_maze(generated, 5, 100, 1), "必须拒绝偶数高度");
    CHECK(!generate_perfect_maze(generated, 101, 5, 1), "必须拒绝超限尺寸");
    CHECK(!generate_perfect_maze(generated, 3, 3, 1), "必须拒绝 S/E 重叠");
}

static void test_generation_and_solver(void) {
    char generated[MAX_ROWS][MAX_COLS + 1] = {{0}};
    char same_seed[MAX_ROWS][MAX_COLS + 1] = {{0}};
    char original[MAX_ROWS][MAX_COLS + 1] = {{0}};

    CHECK(generate_perfect_maze(generated, 99, 99, 123456),
          "应能生成最大奇数尺寸迷宫");
    CHECK(generate_perfect_maze(same_seed, 99, 99, 123456),
          "相同种子应能再次生成迷宫");
    CHECK(memcmp(generated, same_seed, sizeof(generated)) == 0,
          "相同种子必须生成相同迷宫");
    check_perfect_maze(generated, 99, 99);

    ROWS = 99;
    COLS = 99;
    memcpy(original, generated, sizeof(original));
    CHECK(solve_maze_bfs(generated, 1, 1), "BFS 必须能解开生成的迷宫");
    CHECK(path_len > 1, "生成迷宫的路径必须包含 S 和 E");
    CHECK(path_x[0] == 97 && path_y[0] == 97, "路径必须从 E 开始记录");
    CHECK(path_x[path_len - 1] == 1 && path_y[path_len - 1] == 1,
          "路径必须以 S 结束记录");
    CHECK(memcmp(original, generated, sizeof(original)) == 0,
          "BFS 不得修改生成的迷宫");
}

static void test_file_round_trip(void) {
    (void)remove(TEST_FILE);
    CHECK(generate_maze_file(TEST_FILE, 31, 21, 42), "应能写入迷宫文件");
    CHECK(load_maze_from_file(TEST_FILE), "加载器必须接受生成的文件");
    CHECK(COLS == 31 && ROWS == 21, "文件中的尺寸必须正确");
    CHECK(solve_maze_bfs(maze, 1, 1), "文件往返后迷宫必须可解");
    CHECK(remove(TEST_FILE) == 0, "应能清理测试文件");
}

int main(void) {
    test_dimensions();
    test_generation_and_solver();
    test_file_round_trip();

    if (failures != 0) {
        fprintf(stderr, "%d generator test(s) failed\n", failures);
        return 1;
    }
    puts("generator tests passed");
    return 0;
}
