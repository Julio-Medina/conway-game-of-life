#ifndef SPARSE_BOARD_H
#define SPARSE_BOARD_H

#include <stddef.h>

typedef struct SparseBoard SparseBoard;

SparseBoard *sparse_board_create(void);
void sparse_board_destroy(SparseBoard *board);

int sparse_board_is_alive(const SparseBoard *board, int x, int y);
int sparse_board_set_alive(SparseBoard *board, int x, int y);
int sparse_board_set_dead(SparseBoard *board, int x, int y);

SparseBoard *sparse_board_step(const SparseBoard *current);
size_t sparse_board_population(const SparseBoard *board);

#endif
