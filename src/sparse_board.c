#include "sparse_board.h"

#include <stdint.h>
#include <stdlib.h>

typedef struct {
    int x;
    int y;
    unsigned char state;
} CellEntry;

struct SparseBoard {
    size_t capacity;
    size_t count;
    CellEntry *entries;
};

#define ENTRY_EMPTY 0
#define ENTRY_OCCUPIED 1
#define ENTRY_DELETED 2

#define INITIAL_CAPACITY 1024
#define MAX_LOAD_PERCENT 70

typedef struct {
    int x;
    int y;
    int count;
    unsigned char state;
} CountEntry;

typedef struct {
    size_t capacity;
    size_t count;
    CountEntry *entries;
} CountMap;

static uint64_t hash_coords(int x, int y) {
    uint64_t ux = (uint32_t)x;
    uint64_t uy = (uint32_t)y;

    uint64_t h = ux;
    h ^= uy + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
    h ^= h >> 30;
    h *= 0xbf58476d1ce4e5b9ULL;
    h ^= h >> 27;
    h *= 0x94d049bb133111ebULL;
    h ^= h >> 31;

    return h;
}

static size_t next_power_of_two(size_t value) {
    size_t result = 1;
    while (result < value) {
        result <<= 1;
    }
    return result;
}

static int sparse_board_init(SparseBoard *board, size_t capacity) {
    board->capacity = next_power_of_two(capacity);
    board->count = 0;
    board->entries = calloc(board->capacity, sizeof(CellEntry));
    return board->entries != NULL;
}

SparseBoard *sparse_board_create(void) {
    SparseBoard *board = malloc(sizeof(SparseBoard));
    if (!board) {
        return NULL;
    }

    if (!sparse_board_init(board, INITIAL_CAPACITY)) {
        free(board);
        return NULL;
    }

    return board;
}

void sparse_board_destroy(SparseBoard *board) {
    if (!board) {
        return;
    }

    free(board->entries);
    free(board);
}

static CellEntry *find_cell_slot(SparseBoard *board, int x, int y, int for_insert) {
    size_t mask = board->capacity - 1;
    size_t index = (size_t)hash_coords(x, y) & mask;
    CellEntry *first_deleted = NULL;

    for (;;) {
        CellEntry *entry = &board->entries[index];

        if (entry->state == ENTRY_EMPTY) {
            if (for_insert && first_deleted) {
                return first_deleted;
            }
            return entry;
        }

        if (entry->state == ENTRY_DELETED) {
            if (for_insert && !first_deleted) {
                first_deleted = entry;
            }
        } else if (entry->x == x && entry->y == y) {
            return entry;
        }

        index = (index + 1) & mask;
    }
}

static int sparse_board_rehash(SparseBoard *board, size_t new_capacity) {
    SparseBoard rebuilt;

    if (!sparse_board_init(&rebuilt, new_capacity)) {
        return 0;
    }

    for (size_t i = 0; i < board->capacity; i++) {
        CellEntry *entry = &board->entries[i];
        if (entry->state == ENTRY_OCCUPIED) {
            sparse_board_set_alive(&rebuilt, entry->x, entry->y);
        }
    }

    free(board->entries);
    board->capacity = rebuilt.capacity;
    board->count = rebuilt.count;
    board->entries = rebuilt.entries;
    return 1;
}

int sparse_board_is_alive(const SparseBoard *board, int x, int y) {
    if (!board) {
        return 0;
    }

    SparseBoard *mutable_board = (SparseBoard *)board;
    CellEntry *entry = find_cell_slot(mutable_board, x, y, 0);
    return entry->state == ENTRY_OCCUPIED;
}

int sparse_board_set_alive(SparseBoard *board, int x, int y) {
    if (!board) {
        return 0;
    }

    if ((board->count + 1) * 100 >= board->capacity * MAX_LOAD_PERCENT) {
        if (!sparse_board_rehash(board, board->capacity * 2)) {
            return 0;
        }
    }

    CellEntry *entry = find_cell_slot(board, x, y, 1);

    if (entry->state != ENTRY_OCCUPIED) {
        board->count++;
    }

    entry->x = x;
    entry->y = y;
    entry->state = ENTRY_OCCUPIED;
    return 1;
}

int sparse_board_set_dead(SparseBoard *board, int x, int y) {
    if (!board) {
        return 0;
    }

    CellEntry *entry = find_cell_slot(board, x, y, 0);

    if (entry->state == ENTRY_OCCUPIED) {
        entry->state = ENTRY_DELETED;
        board->count--;
    }

    return 1;
}

size_t sparse_board_population(const SparseBoard *board) {
    if (!board) {
        return 0;
    }

    return board->count;
}

static int count_map_init(CountMap *map, size_t capacity) {
    map->capacity = next_power_of_two(capacity);
    map->count = 0;
    map->entries = calloc(map->capacity, sizeof(CountEntry));
    return map->entries != NULL;
}

static void count_map_destroy(CountMap *map) {
    free(map->entries);
    map->entries = NULL;
    map->capacity = 0;
    map->count = 0;
}

static CountEntry *find_count_slot(CountMap *map, int x, int y) {
    size_t mask = map->capacity - 1;
    size_t index = (size_t)hash_coords(x, y) & mask;

    for (;;) {
        CountEntry *entry = &map->entries[index];

        if (entry->state == ENTRY_EMPTY || (entry->x == x && entry->y == y)) {
            return entry;
        }

        index = (index + 1) & mask;
    }
}

static int count_map_rehash(CountMap *map, size_t new_capacity) {
    CountMap rebuilt;

    if (!count_map_init(&rebuilt, new_capacity)) {
        return 0;
    }

    for (size_t i = 0; i < map->capacity; i++) {
        CountEntry *entry = &map->entries[i];
        if (entry->state == ENTRY_OCCUPIED) {
            CountEntry *slot = find_count_slot(&rebuilt, entry->x, entry->y);
            slot->x = entry->x;
            slot->y = entry->y;
            slot->count = entry->count;
            slot->state = ENTRY_OCCUPIED;
            rebuilt.count++;
        }
    }

    free(map->entries);
    map->capacity = rebuilt.capacity;
    map->count = rebuilt.count;
    map->entries = rebuilt.entries;
    return 1;
}

static int count_map_increment(CountMap *map, int x, int y) {
    if ((map->count + 1) * 100 >= map->capacity * MAX_LOAD_PERCENT) {
        if (!count_map_rehash(map, map->capacity * 2)) {
            return 0;
        }
    }

    CountEntry *entry = find_count_slot(map, x, y);

    if (entry->state != ENTRY_OCCUPIED) {
        entry->x = x;
        entry->y = y;
        entry->count = 0;
        entry->state = ENTRY_OCCUPIED;
        map->count++;
    }

    entry->count++;
    return 1;
}

SparseBoard *sparse_board_step(const SparseBoard *current) {
    if (!current) {
        return NULL;
    }

    SparseBoard *next = sparse_board_create();
    if (!next) {
        return NULL;
    }

    size_t expected_neighbors = current->count * 8;
    if (expected_neighbors < INITIAL_CAPACITY) {
        expected_neighbors = INITIAL_CAPACITY;
    }

    CountMap counts;
    if (!count_map_init(&counts, expected_neighbors * 2)) {
        sparse_board_destroy(next);
        return NULL;
    }

    for (size_t i = 0; i < current->capacity; i++) {
        const CellEntry *cell = &current->entries[i];

        if (cell->state != ENTRY_OCCUPIED) {
            continue;
        }

        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                if (dx == 0 && dy == 0) {
                    continue;
                }

                if (!count_map_increment(&counts, cell->x + dx, cell->y + dy)) {
                    count_map_destroy(&counts);
                    sparse_board_destroy(next);
                    return NULL;
                }
            }
        }
    }

    for (size_t i = 0; i < counts.capacity; i++) {
        CountEntry *entry = &counts.entries[i];

        if (entry->state != ENTRY_OCCUPIED) {
            continue;
        }

        int alive = sparse_board_is_alive(current, entry->x, entry->y);

        if (entry->count == 3 || (alive && entry->count == 2)) {
            if (!sparse_board_set_alive(next, entry->x, entry->y)) {
                count_map_destroy(&counts);
                sparse_board_destroy(next);
                return NULL;
            }
        }
    }

    count_map_destroy(&counts);
    return next;
}
