#ifndef IO_H
#define IO_H

#include "life.h"

Board *load_pattern_file(const char *path, size_t min_rows, size_t min_cols);

#endif
