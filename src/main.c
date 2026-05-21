#include "life.h"
#include "io.h"
#include "terminal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEFAULT_ROWS 25
#define DEFAULT_COLS 80
#define DEFAULT_DELAY_MS 150

typedef struct {
    const char *file_path;
    int delay_ms;
    int max_generations;
} Config;

static void print_usage(const char *program_name) {
    fprintf(stderr,
        "Usage: %s --file <pattern-file> [--delay <ms>] [--max-generations <n>]\n\n"
        "Controls:\n"
        "  q  quit\n"
        "  +  increase speed\n"
        "  -  decrease speed\n"
        "  p  pause/resume\n"
        "  n  step once while paused\n"
        "  r  reset\n",
        program_name
    );
}

static int parse_args(int argc, char **argv, Config *config) {
    config->file_path = NULL;
    config->delay_ms = DEFAULT_DELAY_MS;
    config->max_generations = -1;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--file") == 0 && i + 1 < argc) {
            config->file_path = argv[++i];
        } else if (strcmp(argv[i], "--delay") == 0 && i + 1 < argc) {
            config->delay_ms = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--max-generations") == 0 && i + 1 < argc) {
            config->max_generations = atoi(argv[++i]);
        } else {
            return 0;
        }
    }

    return config->file_path != NULL;
}

static void render_board(const Board *board, int generation, int delay_ms, int paused) {
    terminal_clear_screen();

    printf("Conway's Game of Life | generation: %d | delay: %d ms | %s\n",
           generation,
           delay_ms,
           paused ? "paused" : "running");

    printf("Controls: q quit | + faster | - slower | p pause | n step | r reset\n\n");

    for (size_t row = 0; row < board->rows; row++) {
        for (size_t col = 0; col < board->cols; col++) {
            putchar(board_get(board, row, col) ? 'O' : ' ');
        }
        putchar('\n');
    }

    fflush(stdout);
}

static void handle_key(
    int key,
    int *running,
    int *paused,
    int *delay_ms,
    Board *current,
    const Board *initial
) {
    switch (key) {
        case 'q':
        case 'Q':
            *running = 0;
            break;

        case '+':
        case '=':
            if (*delay_ms > 20) {
                *delay_ms -= 20;
            }
            break;

        case '-':
        case '_':
            *delay_ms += 20;
            break;

        case 'p':
        case 'P':
            *paused = !*paused;
            break;

        case 'r':
        case 'R':
            board_copy(current, initial);
            break;

        default:
            break;
    }
}

int main(int argc, char **argv) {
    Config config;

    if (!parse_args(argc, argv, &config)) {
        print_usage(argv[0]);
        return 1;
    }

    Board *initial = load_pattern_file(config.file_path, DEFAULT_ROWS, DEFAULT_COLS);
    if (!initial) {
        fprintf(stderr, "Error: could not load pattern file: %s\n", config.file_path);
        return 1;
    }

    Board *current = board_clone(initial);
    Board *next = board_create(current->rows, current->cols);

    if (!current || !next) {
        fprintf(stderr, "Error: could not allocate board memory.\n");
        board_destroy(initial);
        board_destroy(current);
        board_destroy(next);
        return 1;
    }

    if (terminal_enable_raw_mode() != 0) {
        fprintf(stderr, "Error: could not enable raw terminal mode.\n");
        board_destroy(initial);
        board_destroy(current);
        board_destroy(next);
        return 1;
    }

    terminal_hide_cursor();

    int running = 1;
    int paused = 0;
    int generation = 0;

    while (running) {
        render_board(current, generation, config.delay_ms, paused);

        int key = terminal_read_key();

        if (key == 'n' || key == 'N') {
            if (paused) {
                board_step(current, next);
                board_copy(current, next);
                board_clear(next);
                generation++;
            }
        } else {
            handle_key(key, &running, &paused, &config.delay_ms, current, initial);
        }

        if (!paused && running) {
            board_step(current, next);
            board_copy(current, next);
            board_clear(next);
            generation++;
        }

        if (config.max_generations >= 0 && generation >= config.max_generations) {
            running = 0;
        }

        usleep((useconds_t)config.delay_ms * 1000);
    }

    terminal_show_cursor();
    terminal_disable_raw_mode();
    terminal_clear_screen();

    board_destroy(initial);
    board_destroy(current);
    board_destroy(next);

    return 0;
}
