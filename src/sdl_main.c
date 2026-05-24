#include "renderer_sdl.h"
#include "sparse_board.h"
#include "viewport.h"

#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_WINDOW_WIDTH 1200
#define DEFAULT_WINDOW_HEIGHT 800
#define DEFAULT_CELL_SIZE 12
#define MIN_CELL_SIZE 2
#define MAX_CELL_SIZE 80

static void load_glider(SparseBoard *board, int x, int y) {
    sparse_board_set_alive(board, x + 1, y + 0);
    sparse_board_set_alive(board, x + 2, y + 1);
    sparse_board_set_alive(board, x + 0, y + 2);
    sparse_board_set_alive(board, x + 1, y + 2);
    sparse_board_set_alive(board, x + 2, y + 2);
}

static void load_blinker(SparseBoard *board, int x, int y) {
    sparse_board_set_alive(board, x - 1, y);
    sparse_board_set_alive(board, x, y);
    sparse_board_set_alive(board, x + 1, y);
}

static void load_demo_pattern(SparseBoard *board) {
    load_glider(board, 0, 0);
    load_blinker(board, -12, -3);
    load_glider(board, 15, 10);
}

static Viewport viewport_from_renderer(const SdlRenderer *renderer, int origin_x, int origin_y) {
    size_t width = (size_t)(renderer->window_width / renderer->cell_size);
    size_t height = (size_t)(renderer->window_height / renderer->cell_size);

    if (width == 0) {
        width = 1;
    }

    if (height == 0) {
        height = 1;
    }

    return viewport_create(origin_x, origin_y, width, height);
}

static void recalculate_viewport_size(Viewport *viewport, const SdlRenderer *renderer) {
    int center_x = viewport->origin_x + (int)(viewport->width / 2);
    int center_y = viewport->origin_y + (int)(viewport->height / 2);

    viewport->width = (size_t)(renderer->window_width / renderer->cell_size);
    viewport->height = (size_t)(renderer->window_height / renderer->cell_size);

    if (viewport->width == 0) {
        viewport->width = 1;
    }

    if (viewport->height == 0) {
        viewport->height = 1;
    }

    viewport_center_on(viewport, center_x, center_y);
}

static void zoom_at_center(SdlRenderer *renderer, Viewport *viewport, int delta) {
    int center_x = viewport->origin_x + (int)(viewport->width / 2);
    int center_y = viewport->origin_y + (int)(viewport->height / 2);

    renderer->cell_size += delta;

    if (renderer->cell_size < MIN_CELL_SIZE) {
        renderer->cell_size = MIN_CELL_SIZE;
    }

    if (renderer->cell_size > MAX_CELL_SIZE) {
        renderer->cell_size = MAX_CELL_SIZE;
    }

    recalculate_viewport_size(viewport, renderer);
    viewport_center_on(viewport, center_x, center_y);
}

static void print_help(void) {
    puts("SDL2 Conway sparse-board demo");
    puts("");
    puts("Controls:");
    puts("  q / Esc       quit");
    puts("  Space / p     pause/resume");
    puts("  n             step once while paused");
    puts("  Arrow/WASD    move camera");
    puts("  Mouse wheel   zoom");
    puts("  z / x         zoom in/out");
    puts("  + / -         faster/slower");
    puts("  0             recenter camera");
}

int main(void) {
    print_help();

    SparseBoard *board = sparse_board_create();
    if (!board) {
        fprintf(stderr, "Could not create sparse board.\n");
        return 1;
    }

    load_demo_pattern(board);

    SdlRenderer renderer = {0};

    if (!sdl_renderer_init(
            &renderer,
            "Conway's Game of Life - Sparse SDL2 Renderer",
            DEFAULT_WINDOW_WIDTH,
            DEFAULT_WINDOW_HEIGHT,
            DEFAULT_CELL_SIZE
        )) {
        sparse_board_destroy(board);
        return 1;
    }

    Viewport viewport = viewport_from_renderer(&renderer, -40, -25);

    int running = 1;
    int paused = 0;
    int step_once = 0;
    int delay_ms = 120;
    Uint32 last_step = SDL_GetTicks();
    unsigned long generation = 0;

    while (running) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }

            if (event.type == SDL_WINDOWEVENT &&
                event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                renderer.window_width = event.window.data1;
                renderer.window_height = event.window.data2;
                recalculate_viewport_size(&viewport, &renderer);
            }

            if (event.type == SDL_MOUSEWHEEL) {
                if (event.wheel.y > 0) {
                    zoom_at_center(&renderer, &viewport, 2);
                } else if (event.wheel.y < 0) {
                    zoom_at_center(&renderer, &viewport, -2);
                }
            }

            if (event.type == SDL_KEYDOWN) {
                SDL_Keycode key = event.key.keysym.sym;

                switch (key) {
                    case SDLK_ESCAPE:
                    case SDLK_q:
                        running = 0;
                        break;

                    case SDLK_SPACE:
                    case SDLK_p:
                        paused = !paused;
                        break;

                    case SDLK_n:
                        step_once = 1;
                        break;

                    case SDLK_UP:
                    case SDLK_w:
                        viewport_move(&viewport, 0, -4);
                        break;

                    case SDLK_DOWN:
                    case SDLK_s:
                        viewport_move(&viewport, 0, 4);
                        break;

                    case SDLK_LEFT:
                    case SDLK_a:
                        viewport_move(&viewport, -6, 0);
                        break;

                    case SDLK_RIGHT:
                    case SDLK_d:
                        viewport_move(&viewport, 6, 0);
                        break;

                    case SDLK_z:
                        zoom_at_center(&renderer, &viewport, 2);
                        break;

                    case SDLK_x:
                        zoom_at_center(&renderer, &viewport, -2);
                        break;

                    case SDLK_EQUALS:
                    case SDLK_PLUS:
                        if (delay_ms > 20) {
                            delay_ms -= 20;
                        }
                        break;

                    case SDLK_MINUS:
                        if (delay_ms < 2000) {
                            delay_ms += 20;
                        }
                        break;

                    case SDLK_0:
                        viewport_center_on(&viewport, 0, 0);
                        break;

                    default:
                        break;
                }
            }
        }

        Uint32 now = SDL_GetTicks();

        if (running && (!paused || step_once) && now - last_step >= (Uint32)delay_ms) {
            SparseBoard *next = sparse_board_step(board);
            sparse_board_destroy(board);

            if (!next) {
                fprintf(stderr, "Could not compute next generation.\n");
                sdl_renderer_destroy(&renderer);
                return 1;
            }

            board = next;
            generation++;
            step_once = 0;
            last_step = now;
        }

        char title[256];
        snprintf(
            title,
            sizeof(title),
            "Conway SDL2 | generation %lu | population %zu | delay %d ms | cell %d px | %s",
            generation,
            sparse_board_population(board),
            delay_ms,
            renderer.cell_size,
            paused ? "paused" : "running"
        );
        SDL_SetWindowTitle(renderer.window, title);

        sdl_renderer_draw_board(&renderer, board, viewport);
        SDL_Delay(8);
    }

    sdl_renderer_destroy(&renderer);
    sparse_board_destroy(board);

    return 0;
}
