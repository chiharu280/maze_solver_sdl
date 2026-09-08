#ifndef SOLVER_H
#define SOLVER_H

#include "maze.h"

/* A simple path cannot contain more cells than the whole maze. */
#define MAX_PATH_LEN (MAX_ROWS * MAX_COLS)

/* The route shared with the renderer; entries are ordered E back to S. */
extern int path_len;
extern int path_x[MAX_PATH_LEN];
extern int path_y[MAX_PATH_LEN];

/* Run recursive DFS. It may alter maze cells while searching and is not shortest. */
int solve_maze(char maze[][MAX_COLS + 1], int x, int y);

/*
 * Use breadth-first search to find the shortest path from (x, y) to 'E'.
 * On success, path_x/path_y contain the path from the end back to the start,
 * matching the format expected by visualization_play_maze().
 */
int solve_maze_bfs(char maze[][MAX_COLS + 1], int x, int y);

#endif
