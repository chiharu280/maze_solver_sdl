#include <stdio.h>
#include "maze.h"
#include "solver.h"
#include "visualize.h"

/*
 * Application entry point.
 *
 * The program loads the maze model from disk, locates its validated start
 * cell, obtains a shortest path with BFS, and hands the result to the SDL
 * presentation layer.  Each stage returns a status so a failure is reported
 * to the operating system instead of being silently ignored.
 */
int main(void) {
    int exit_code = 1;

    /* The loader also validates dimensions, row contents, and the S/E cells. */
    if (!load_maze_from_file("assets/maze.txt")) {
        return exit_code;
    }

    /* Keep start discovery separate from file I/O for a reusable maze API. */
    int start_x;
    int start_y;
    if (!find_maze_marker('S', &start_x, &start_y)) {
        printf("找不到起点 S\n");
        return exit_code;
    }

    /*
     * BFS returns the shortest route in reverse order: E first and S last.
     * This ordering lets the renderer animate naturally by iterating backward.
     */
    if (!solve_maze_bfs(maze, start_x, start_y)) {
        printf("\n未找到路径。\n");
        return exit_code;
    }

    /* SDL resources now live for the application, not for one animation call. */
    if (!visualization_init(COLS, ROWS)) {
        visualization_shutdown();
        return exit_code;
    }

    VisualizationResult result =
        visualization_play_maze(maze, path_x, path_y, path_len);
    if (result == VISUALIZATION_FINISHED) {
        /* A future menu loop will replace this temporary final-frame wait. */
        visualization_wait_for_close();
        exit_code = 0;
    } else if (result == VISUALIZATION_QUIT) {
        /* Closing the window during animation is a normal user exit. */
        exit_code = 0;
    }

    visualization_shutdown();
    return exit_code;
}
