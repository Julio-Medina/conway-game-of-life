#include "life.h"

#include <stdlib.h>
#include <string.h>

static size_t index_of(const Board *board, size_t row, size_t col) {
    return row * board->cols + col;
}

Board *board_create(size_t rows, size_t cols) {
    Board *board = malloc(sizeof(Board));
    if (!board) {
        return NULL;
    }

    board->rows = rows;
    board->cols = cols;
    board->cells = calloc(rows * cols, sizeof(unsigned char));

    if (!board->cells) {
        free(board);
        return NULL;
    }

    return board;
}

Board *board_clone(const Board *src) {
    if (!src) {
        return NULL;
    }

    Board *copy = board_create(src->rows, src->cols);
    if (!copy) {
        return NULL;
    }

    memcpy(copy->cells, src->cells, src->rows * src->cols);
    return copy;
}

void board_destroy(Board *board) {
    if (!board) {
        return;
    }

    free(board->cells);
    free(board);
}

int board_get(const Board *board, size_t row, size_t col) {
    if (!board || row >= board->rows || col >= board->cols) {
        return 0;
    }

    return board->cells[index_of(board, row, col)] ? 1 : 0;
}

void board_set(Board *board, size_t row, size_t col, int value) {
    if (!board || row >= board->rows || col >= board->cols) {
        return;
    }

    board->cells[index_of(board, row, col)] = value ? 1 : 0;
}

static int count_neighbors(const Board *board, size_t row, size_t col) {
    int count = 0;

    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) {
                continue;
            }

            int rr = (int)row + dr;
            int cc = (int)col + dc;

            if (rr >= 0 && cc >= 0) {
                count += board_get(board, (size_t)rr, (size_t)cc);
            }
        }
    }

    return count;
}

void board_step(const Board *current, Board *next) {
    if (!current || !next) {
        return;
    }

    for (size_t row = 0; row < current->rows; row++) {
        for (size_t col = 0; col < current->cols; col++) {
            int alive = board_get(current, row, col);
            int neighbors = count_neighbors(current, row, col);

            int next_alive = 0;

            if (alive && (neighbors == 2 || neighbors == 3)) {
                next_alive = 1;
            } else if (!alive && neighbors == 3) {
                next_alive = 1;
            }

            board_set(next, row, col, next_alive);
        }
    }
}

void board_clear(Board *board) {
    if (!board) {
        return;
    }

    memset(board->cells, 0, board->rows * board->cols);
}

void board_copy(Board *dest, const Board *src) {
    if (!dest || !src) {
        return;
    }

    if (dest->rows != src->rows || dest->cols != src->cols) {
        return;
    }

    memcpy(dest->cells, src->cells, src->rows * src->cols);
}
