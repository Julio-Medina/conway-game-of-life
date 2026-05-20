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
    size_t rows = 0;
    size_t cols = 0;

    while (fgets(line, sizeof(line), file)) {
        size_t len = strcspn(line, "\r\n");
        if (len > cols) {
            cols = len;
        }
        rows++;
    }

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

    size_t row = 0;
    while (fgets(line, sizeof(line), file) && row < rows) {
        size_t len = strcspn(line, "\r\n");

        for (size_t col = 0; col < len && col < cols; col++) {
            if (line[col] == 'O' || line[col] == 'o' || line[col] == '1' || line[col] == '#') {
                board_set(board, row, col, 1);
            }
        }

        row++;
    }

    fclose(file);
    return board;
}
