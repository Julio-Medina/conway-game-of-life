#include "sparse_board.h"
#include "viewport.h"

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define KEY_NONE 0
#define KEY_QUIT 1
#define KEY_PAUSE 2
#define KEY_STEP 3
#define KEY_UP 4
#define KEY_DOWN 5
#define KEY_LEFT 6
#define KEY_RIGHT 7
#define KEY_ZOOM_IN 8
#define KEY_ZOOM_OUT 9
#define KEY_FASTER 10
#define KEY_SLOWER 11
#define KEY_RECENTER 12

#define MIN_VIEW_WIDTH 10
#define MIN_VIEW_HEIGHT 6
#define MAX_VIEW_WIDTH 160
#define MAX_VIEW_HEIGHT 80

static struct termios original_termios;
static int raw_mode_enabled = 0;

static int enable_raw_mode(void) {
    if (tcgetattr(STDIN_FILENO, &original_termios) == -1) {
        return 0;
    }

    struct termios raw = original_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
        return 0;
    }

    raw_mode_enabled = 1;
    return 1;
}

static void disable_raw_mode(void) {
    if (raw_mode_enabled) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
        raw_mode_enabled = 0;
    }
}

static void sleep_ms(int milliseconds) {
    if (milliseconds <= 0) {
        return;
    }

    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (long)(milliseconds % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

static void clear_screen(void) {
    printf("\033[H\033[J");
}

static void hide_cursor(void) {
    printf("\033[?25l");
}

static void show_cursor(void) {
    printf("\033[?25h");
}

static int read_key(void) {
    unsigned char ch;

    if (read(STDIN_FILENO, &ch, 1) != 1) {
        return KEY_NONE;
    }

    if (ch == 'q' || ch == 'Q') {
        return KEY_QUIT;
    }

    if (ch == 'p' || ch == 'P' || ch == ' ') {
        return KEY_PAUSE;
    }

    if (ch == 'n' || ch == 'N') {
        return KEY_STEP;
    }

    if (ch == '0') {
        return KEY_RECENTER;
    }

    if (ch == '+') {
        return KEY_FASTER;
    }

    if (ch == '-') {
        return KEY_SLOWER;
    }

    if (ch == 'z' || ch == 'Z') {
        return KEY_ZOOM_IN;
    }

    if (ch == 'x' || ch == 'X') {
        return KEY_ZOOM_OUT;
    }

    if (ch == 'w' || ch == 'W') {
        return KEY_UP;
    }

    if (ch == 's' || ch == 'S') {
        return KEY_DOWN;
    }

    if (ch == 'a' || ch == 'A') {
        return KEY_LEFT;
    }

    if (ch == 'd' || ch == 'D') {
        return KEY_RIGHT;
    }

    if (ch == '\033') {
        unsigned char seq[2];

        if (read(STDIN_FILENO, &seq[0], 1) != 1) {
            return KEY_NONE;
        }

        if (read(STDIN_FILENO, &seq[1], 1) != 1) {
            return KEY_NONE;
        }

        if (seq[0] == '[') {
            switch (seq[1]) {
                case 'A':
                    return KEY_UP;
                case 'B':
                    return KEY_DOWN;
                case 'C':
                    return KEY_RIGHT;
                case 'D':
                    return KEY_LEFT;
                default:
                    return KEY_NONE;
            }
        }
    }

    return KEY_NONE;
}

static void resize_viewport(Viewport *viewport, int delta_width, int delta_height) {
    int center_x = viewport->origin_x + (int)(viewport->width / 2);
    int center_y = viewport->origin_y + (int)(viewport->height / 2);

    int next_width = (int)viewport->width + delta_width;
    int next_height = (int)viewport->height + delta_height;

    if (next_width < MIN_VIEW_WIDTH) {
        next_width = MIN_VIEW_WIDTH;
    }

    if (next_height < MIN_VIEW_HEIGHT) {
        next_height = MIN_VIEW_HEIGHT;
    }

    if (next_width > MAX_VIEW_WIDTH) {
        next_width = MAX_VIEW_WIDTH;
    }

    if (next_height > MAX_VIEW_HEIGHT) {
        next_height = MAX_VIEW_HEIGHT;
    }

    viewport->width = (size_t)next_width;
    viewport->height = (size_t)next_height;
    viewport_center_on(viewport, center_x, center_y);
}

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

static void handle_key(int key, Viewport *viewport, int *running, int *paused, int *step_once, int *delay_ms) {
    switch (key) {
        case KEY_QUIT:
            *running = 0;
            break;

        case KEY_PAUSE:
            *paused = !*paused;
            break;

        case KEY_STEP:
            *step_once = 1;
            break;

        case KEY_UP:
            viewport_move(viewport, 0, -2);
            break;

        case KEY_DOWN:
            viewport_move(viewport, 0, 2);
            break;

        case KEY_LEFT:
            viewport_move(viewport, -4, 0);
            break;

        case KEY_RIGHT:
            viewport_move(viewport, 4, 0);
            break;

        case KEY_ZOOM_IN:
            resize_viewport(viewport, -8, -4);
            break;

        case KEY_ZOOM_OUT:
            resize_viewport(viewport, 8, 4);
            break;

        case KEY_FASTER:
            if (*delay_ms > 20) {
                *delay_ms -= 20;
            }
            break;

        case KEY_SLOWER:
            if (*delay_ms < 2000) {
                *delay_ms += 20;
            }
            break;

        case KEY_RECENTER:
            viewport_center_on(viewport, 0, 0);
            break;

        default:
            break;
    }
}

int main(int argc, char **argv) {
    int generations = -1;
    int delay_ms = 120;

    if (argc >= 2) {
        generations = atoi(argv[1]);
    }

    if (argc >= 3) {
        delay_ms = atoi(argv[2]);
    }

    SparseBoard *board = sparse_board_create();
    if (!board) {
        fprintf(stderr, "Could not create sparse board.\n");
        return 1;
    }

    load_glider(board, 0, 0);
    load_blinker(board, -12, -3);

    Viewport viewport = viewport_create(-20, -10, 60, 24);

    if (!enable_raw_mode()) {
        sparse_board_destroy(board);
        fprintf(stderr, "Could not enable raw terminal mode.\n");
        return 1;
    }

    hide_cursor();

    int running = 1;
    int paused = 0;
    int generation = 0;

    while (running) {
        int step_once = 0;
        int key;

        while ((key = read_key()) != KEY_NONE) {
            handle_key(key, &viewport, &running, &paused, &step_once, &delay_ms);
        }

        clear_screen();

        char *rendered = viewport_render_to_string(board, viewport);
        if (!rendered) {
            show_cursor();
            disable_raw_mode();
            sparse_board_destroy(board);
            fprintf(stderr, "Could not render viewport.\n");
            return 1;
        }

        printf("Sparse infinite board demo | generation: %d | population: %zu | delay: %d ms | %s\n",
               generation,
               sparse_board_population(board),
               delay_ms,
               paused ? "paused" : "running");
        printf("Viewport origin: (%d, %d), size: %zux%zu\n",
               viewport.origin_x,
               viewport.origin_y,
               viewport.width,
               viewport.height);
        printf("Controls: arrows/WASD move | z zoom in | x zoom out | +/- speed | p pause | n step | 0 recenter | q quit\n\n");
        printf("%s", rendered);
        free(rendered);

        if (running && (!paused || step_once)) {
            SparseBoard *next = sparse_board_step(board);
            sparse_board_destroy(board);

            if (!next) {
                show_cursor();
                disable_raw_mode();
                fprintf(stderr, "Could not compute next generation.\n");
                return 1;
            }

            board = next;
            generation++;
        }

        if (generations >= 0 && generation >= generations) {
            running = 0;
        }

        sleep_ms(delay_ms);
    }

    show_cursor();
    disable_raw_mode();
    clear_screen();

    sparse_board_destroy(board);
    return 0;
}
