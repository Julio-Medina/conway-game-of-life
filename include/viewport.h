#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <stddef.h>

#include "sparse_board.h"

typedef struct {
    int origin_x;
    int origin_y;
    size_t width;
    size_t height;
} Viewport;

Viewport viewport_create(int origin_x, int origin_y, size_t width, size_t height);
int viewport_contains(Viewport viewport, int x, int y);
void viewport_move(Viewport *viewport, int dx, int dy);
void viewport_center_on(Viewport *viewport, int center_x, int center_y);

char *viewport_render_to_string(const SparseBoard *board, Viewport viewport);

#endif
