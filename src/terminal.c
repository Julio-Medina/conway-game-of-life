#include "terminal.h"

#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>

static struct termios original_termios;
static int raw_mode_enabled = 0;

int terminal_enable_raw_mode(void) {
    if (tcgetattr(STDIN_FILENO, &original_termios) == -1) {
        return -1;
    }

    struct termios raw = original_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
        return -1;
    }

    raw_mode_enabled = 1;
    return 0;
}

void terminal_disable_raw_mode(void) {
    if (raw_mode_enabled) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
        raw_mode_enabled = 0;
    }
}

int terminal_read_key(void) {
    fd_set set;
    struct timeval timeout;

    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);

    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    int ready = select(STDIN_FILENO + 1, &set, NULL, NULL, &timeout);
    if (ready > 0) {
        unsigned char ch;
        if (read(STDIN_FILENO, &ch, 1) == 1) {
            return ch;
        }
    }

    return -1;
}

void terminal_clear_screen(void) {
    printf("\033[H\033[J");
}

void terminal_hide_cursor(void) {
    printf("\033[?25l");
}

void terminal_show_cursor(void) {
    printf("\033[?25h");
}
