#ifndef LIFE_H
#define LIFE_H

#include <stddef.h>

typedef struct {
    size_t rows;
    size_t cols;
    unsigned char *cells;
} Board;

Board *board_create(size_t rows, size_t cols);
Board *board_clone(const Board *src);
void board_destroy(Board *board);

int board_get(const Board *board, size_t row, size_t col);
void board_set(Board *board, size_t row, size_t col, int value);

void board_step(const Board *current, Board *next);
void board_clear(Board *board);
void board_copy(Board *dest, const Board *src);

#endif
