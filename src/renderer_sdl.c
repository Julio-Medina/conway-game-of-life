#include "renderer_sdl.h"

#include <stdio.h>

typedef struct {
    SdlRenderer *renderer;
    Viewport viewport;
} DrawContext;

static void draw_live_cell(int x, int y, void *user_data) {
    DrawContext *context = user_data;
    SdlRenderer *renderer = context->renderer;

    if (!viewport_contains(context->viewport, x, y)) {
        return;
    }

    int screen_x = (x - context->viewport.origin_x) * renderer->cell_size;
    int screen_y = (y - context->viewport.origin_y) * renderer->cell_size;

    SDL_Rect rect;
    rect.x = screen_x;
    rect.y = screen_y;
    rect.w = renderer->cell_size;
    rect.h = renderer->cell_size;

    SDL_RenderFillRect(renderer->renderer, &rect);
}

int sdl_renderer_init(
    SdlRenderer *renderer,
    const char *title,
    int window_width,
    int window_height,
    int cell_size
) {
    if (!renderer || window_width <= 0 || window_height <= 0 || cell_size <= 0) {
        return 0;
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 0;
    }

    renderer->window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    if (!renderer->window) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }

    renderer->renderer = SDL_CreateRenderer(
        renderer->window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!renderer->renderer) {
        fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(renderer->window);
        SDL_Quit();
        return 0;
    }

    renderer->window_width = window_width;
    renderer->window_height = window_height;
    renderer->cell_size = cell_size;

    return 1;
}

void sdl_renderer_destroy(SdlRenderer *renderer) {
    if (!renderer) {
        return;
    }

    if (renderer->renderer) {
        SDL_DestroyRenderer(renderer->renderer);
        renderer->renderer = NULL;
    }

    if (renderer->window) {
        SDL_DestroyWindow(renderer->window);
        renderer->window = NULL;
    }

    SDL_Quit();
}

void sdl_renderer_draw_board(
    SdlRenderer *renderer,
    const SparseBoard *board,
    Viewport viewport
) {
    if (!renderer || !renderer->renderer || !board) {
        return;
    }

    SDL_SetRenderDrawColor(renderer->renderer, 12, 12, 12, 255);
    SDL_RenderClear(renderer->renderer);

    SDL_SetRenderDrawColor(renderer->renderer, 45, 45, 45, 255);

    for (int x = 0; x <= renderer->window_width; x += renderer->cell_size) {
        SDL_RenderDrawLine(renderer->renderer, x, 0, x, renderer->window_height);
    }

    for (int y = 0; y <= renderer->window_height; y += renderer->cell_size) {
        SDL_RenderDrawLine(renderer->renderer, 0, y, renderer->window_width, y);
    }

    SDL_SetRenderDrawColor(renderer->renderer, 230, 230, 230, 255);

    DrawContext context;
    context.renderer = renderer;
    context.viewport = viewport;

    sparse_board_for_each_live_cell(board, draw_live_cell, &context);

    SDL_RenderPresent(renderer->renderer);
}
