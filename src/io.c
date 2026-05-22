#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

Board *load_pattern_file(const char *path, size_t min_rows, size_t min_cols) {
    FILE *file = fopen(path, "r");
    if (!file) {
        return NULL;
    }

    char line[MAX_LINE];
    size_t pattern_rows = 0;
    size_t pattern_cols = 0;

    while (fgets(line, sizeof(line), file)) {
        size_t len = strcspn(line, "\r\n");
        if (len > pattern_cols) {
            pattern_cols = len;
        }
        pattern_rows++;
    }

    size_t rows = pattern_rows;
    size_t cols = pattern_cols;

    if (rows < min_rows) {
        rows = min_rows;
    }

    if (cols < min_cols) {
        cols = min_cols;
    }

    Board *board = board_create(rows, cols);
    if (!board) {
        fclose(file);
        return NULL;
    }

    rewind(file);

    /*
     * Center small pattern files inside the allocated board.
     * Without this, patterns such as a one-line blinker start at row 0,
     * column 0, where they are easy to miss and immediately interact with
     * the edge/wrapping behavior.
     */
    size_t row_offset = 0;
    size_t col_offset = 0;

    if (rows > pattern_rows) {
        row_offset = (rows - pattern_rows) / 2;
    }

    if (cols > pattern_cols) {
        col_offset = (cols - pattern_cols) / 2;
    }

    size_t row = 0;
    while (fgets(line, sizeof(line), file) && row < pattern_rows) {
        size_t len = strcspn(line, "\r\n");

        for (size_t col = 0; col < len && col < pattern_cols; col++) {
            if (line[col] == 'O' || line[col] == 'o' || line[col] == '1' || line[col] == '#') {
                board_set(board, row + row_offset, col + col_offset, 1);
            }
        }

        row++;
    }

    fclose(file);
    return board;
}
