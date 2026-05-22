#include "viewport.h"

#include <stdlib.h>

typedef struct {
    Viewport viewport;
    char *buffer;
    size_t stride;
} RenderContext;

Viewport viewport_create(int origin_x, int origin_y, size_t width, size_t height) {
    Viewport viewport;
    viewport.origin_x = origin_x;
    viewport.origin_y = origin_y;
    viewport.width = width;
    viewport.height = height;
    return viewport;
}

int viewport_contains(Viewport viewport, int x, int y) {
    int max_x = viewport.origin_x + (int)viewport.width;
    int max_y = viewport.origin_y + (int)viewport.height;

    return x >= viewport.origin_x &&
           y >= viewport.origin_y &&
           x < max_x &&
           y < max_y;
}

void viewport_move(Viewport *viewport, int dx, int dy) {
    if (!viewport) {
        return;
    }

    viewport->origin_x += dx;
    viewport->origin_y += dy;
}

void viewport_center_on(Viewport *viewport, int center_x, int center_y) {
    if (!viewport) {
        return;
    }

    viewport->origin_x = center_x - (int)(viewport->width / 2);
    viewport->origin_y = center_y - (int)(viewport->height / 2);
}

static void draw_live_cell(int x, int y, void *user_data) {
    RenderContext *context = user_data;

    if (!viewport_contains(context->viewport, x, y)) {
        return;
    }

    size_t screen_x = (size_t)(x - context->viewport.origin_x);
    size_t screen_y = (size_t)(y - context->viewport.origin_y);
    size_t index = screen_y * context->stride + screen_x;

    context->buffer[index] = 'O';
}

char *viewport_render_to_string(const SparseBoard *board, Viewport viewport) {
    if (!board || viewport.width == 0 || viewport.height == 0) {
        return NULL;
    }

    size_t stride = viewport.width + 1;
    size_t total = stride * viewport.height + 1;

    char *buffer = malloc(total);
    if (!buffer) {
        return NULL;
    }

    for (size_t y = 0; y < viewport.height; y++) {
        for (size_t x = 0; x < viewport.width; x++) {
            buffer[y * stride + x] = '.';
        }

        buffer[y * stride + viewport.width] = '\n';
    }

    buffer[total - 1] = '\0';

    RenderContext context;
    context.viewport = viewport;
    context.buffer = buffer;
    context.stride = stride;

    sparse_board_for_each_live_cell(board, draw_live_cell, &context);

    return buffer;
}
