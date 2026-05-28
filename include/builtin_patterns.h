#ifndef BUILTIN_PATTERNS_H
#define BUILTIN_PATTERNS_H

#include <stddef.h>
#include <stdio.h>

#include "sparse_board.h"

typedef struct {
    const char *name;
    const char *description;
} BuiltinPatternInfo;

size_t builtin_pattern_count(void);
const BuiltinPatternInfo *builtin_pattern_info(size_t index);
void builtin_pattern_print_list(FILE *stream);

int builtin_pattern_load(
    SparseBoard *board,
    const char *name,
    int origin_x,
    int origin_y
);

#endif
