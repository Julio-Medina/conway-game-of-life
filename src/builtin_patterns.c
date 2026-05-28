#include "builtin_patterns.h"

#include <string.h>

typedef struct {
    int x;
    int y;
} PatternCell;

typedef struct {
    const char *name;
    const char *description;
    const PatternCell *cells;
    size_t cell_count;
} BuiltinPattern;

#define CELL_COUNT(cells) (sizeof(cells) / sizeof((cells)[0]))

static const PatternCell GLIDER[] = {
    {1, 0}, {2, 1}, {0, 2}, {1, 2}, {2, 2}
};

static const PatternCell BLINKER[] = {
    {-1, 0}, {0, 0}, {1, 0}
};

static const PatternCell TOAD[] = {
    {1, 0}, {2, 0}, {3, 0},
    {0, 1}, {1, 1}, {2, 1}
};

static const PatternCell BEACON[] = {
    {0, 0}, {1, 0}, {0, 1}, {1, 1},
    {2, 2}, {3, 2}, {2, 3}, {3, 3}
};

static const PatternCell CLOCK[] = {
    {1, 0}, {2, 0},
    {0, 1}, {1, 1},
    {2, 2}, {3, 2},
    {1, 3}, {2, 3}
};

static const PatternCell PULSAR[] = {
    {2, 0}, {3, 0}, {4, 0}, {8, 0}, {9, 0}, {10, 0},
    {0, 2}, {5, 2}, {7, 2}, {12, 2},
    {0, 3}, {5, 3}, {7, 3}, {12, 3},
    {0, 4}, {5, 4}, {7, 4}, {12, 4},
    {2, 5}, {3, 5}, {4, 5}, {8, 5}, {9, 5}, {10, 5},
    {2, 7}, {3, 7}, {4, 7}, {8, 7}, {9, 7}, {10, 7},
    {0, 8}, {5, 8}, {7, 8}, {12, 8},
    {0, 9}, {5, 9}, {7, 9}, {12, 9},
    {0, 10}, {5, 10}, {7, 10}, {12, 10},
    {2, 12}, {3, 12}, {4, 12}, {8, 12}, {9, 12}, {10, 12}
};

static const PatternCell PENTADECATHLON[] = {
    {1, 0}, {2, 0}, {3, 0},
    {0, 1}, {4, 1},
    {1, 2}, {2, 2}, {3, 2},
    {1, 3}, {2, 3}, {3, 3},
    {1, 4}, {2, 4}, {3, 4},
    {0, 5}, {4, 5},
    {1, 6}, {2, 6}, {3, 6}
};

static const PatternCell LWSS[] = {
    {1, 0}, {4, 0},
    {0, 1},
    {0, 2}, {4, 2},
    {0, 3}, {1, 3}, {2, 3}, {3, 3}
};

static const PatternCell R_PENTOMINO[] = {
    {1, 0}, {2, 0},
    {0, 1}, {1, 1},
    {1, 2}
};

static const PatternCell ACORN[] = {
    {1, 0},
    {3, 1},
    {0, 2}, {1, 2}, {4, 2}, {5, 2}, {6, 2}
};

static const PatternCell DIEHARD[] = {
    {6, 0},
    {0, 1}, {1, 1},
    {1, 2}, {5, 2}, {6, 2}, {7, 2}
};

static const PatternCell GOSPER_GLIDER_GUN[] = {
    {24, 0},
    {22, 1}, {24, 1},
    {12, 2}, {13, 2}, {20, 2}, {21, 2}, {34, 2}, {35, 2},
    {11, 3}, {15, 3}, {20, 3}, {21, 3}, {34, 3}, {35, 3},
    {0, 4}, {1, 4}, {10, 4}, {16, 4}, {20, 4}, {21, 4},
    {0, 5}, {1, 5}, {10, 5}, {14, 5}, {16, 5}, {17, 5}, {22, 5}, {24, 5},
    {10, 6}, {16, 6}, {24, 6},
    {11, 7}, {15, 7},
    {12, 8}, {13, 8}
};

static const BuiltinPattern PATTERNS[] = {
    {"glider", "Small spaceship that travels diagonally.", GLIDER, CELL_COUNT(GLIDER)},
    {"blinker", "Period-2 oscillator with three cells.", BLINKER, CELL_COUNT(BLINKER)},
    {"toad", "Period-2 oscillator.", TOAD, CELL_COUNT(TOAD)},
    {"beacon", "Period-2 oscillator made from two blocks.", BEACON, CELL_COUNT(BEACON)},
    {"clock", "Small period-2 oscillator.", CLOCK, CELL_COUNT(CLOCK)},
    {"pulsar", "Large period-3 oscillator.", PULSAR, CELL_COUNT(PULSAR)},
    {"pentadecathlon", "Classic period-15 oscillator.", PENTADECATHLON, CELL_COUNT(PENTADECATHLON)},
    {"lwss", "Lightweight spaceship.", LWSS, CELL_COUNT(LWSS)},
    {"r-pentomino", "Famous methuselah that evolves for many generations.", R_PENTOMINO, CELL_COUNT(R_PENTOMINO)},
    {"acorn", "Long-lived methuselah with complex growth.", ACORN, CELL_COUNT(ACORN)},
    {"diehard", "Eventually vanishing long-lived pattern.", DIEHARD, CELL_COUNT(DIEHARD)},
    {"gosper-gun", "Gosper glider gun that emits gliders.", GOSPER_GLIDER_GUN, CELL_COUNT(GOSPER_GLIDER_GUN)}
};

size_t builtin_pattern_count(void) {
    return CELL_COUNT(PATTERNS);
}

const BuiltinPatternInfo *builtin_pattern_info(size_t index) {
    static BuiltinPatternInfo info;

    if (index >= builtin_pattern_count()) {
        return NULL;
    }

    info.name = PATTERNS[index].name;
    info.description = PATTERNS[index].description;
    return &info;
}

void builtin_pattern_print_list(FILE *stream) {
    if (!stream) {
        return;
    }

    fprintf(stream, "Available built-in patterns:\n");

    for (size_t i = 0; i < builtin_pattern_count(); i++) {
        fprintf(stream, "  %-16s %s\n", PATTERNS[i].name, PATTERNS[i].description);
    }

    fprintf(stream, "  %-16s Curated scene containing multiple patterns.\n", "all");
}

int builtin_pattern_load(
    SparseBoard *board,
    const char *name,
    int origin_x,
    int origin_y
) {
    if (!board || !name) {
        return 0;
    }

    for (size_t i = 0; i < builtin_pattern_count(); i++) {
        const BuiltinPattern *pattern = &PATTERNS[i];

        if (strcmp(pattern->name, name) != 0) {
            continue;
        }

        for (size_t j = 0; j < pattern->cell_count; j++) {
            if (!sparse_board_set_alive(
                    board,
                    origin_x + pattern->cells[j].x,
                    origin_y + pattern->cells[j].y
                )) {
                return 0;
            }
        }

        return 1;
    }

    return 0;
}
