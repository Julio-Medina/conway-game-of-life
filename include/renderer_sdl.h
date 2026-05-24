#ifndef RENDERER_SDL_H
#define RENDERER_SDL_H

#include <SDL2/SDL.h>

#include "sparse_board.h"
#include "viewport.h"

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    int window_width;
    int window_height;
    int cell_size;
} SdlRenderer;

int sdl_renderer_init(
    SdlRenderer *renderer,
    const char *title,
    int window_width,
    int window_height,
    int cell_size
);

void sdl_renderer_destroy(SdlRenderer *renderer);
void sdl_renderer_draw_board(
    SdlRenderer *renderer,
    const SparseBoard *board,
    Viewport viewport
);

#endif
