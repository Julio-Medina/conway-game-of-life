#include "sparse_board.h"

#include <assert.h>
#include <stdio.h>

static void test_empty_board_has_zero_population(void) {
    SparseBoard *board = sparse_board_create();

    assert(board != NULL);
    assert(sparse_board_population(board) == 0);
    assert(!sparse_board_is_alive(board, 0, 0));

    sparse_board_destroy(board);
}

static void test_set_alive_and_dead_with_negative_coordinates(void) {
    SparseBoard *board = sparse_board_create();

    assert(board != NULL);
    assert(sparse_board_set_alive(board, -10, 25));
    assert(sparse_board_is_alive(board, -10, 25));
    assert(sparse_board_population(board) == 1);

    assert(sparse_board_set_dead(board, -10, 25));
    assert(!sparse_board_is_alive(board, -10, 25));
    assert(sparse_board_population(board) == 0);

    sparse_board_destroy(board);
}

static void test_block_still_life_survives(void) {
    SparseBoard *board = sparse_board_create();

    sparse_board_set_alive(board, 0, 0);
    sparse_board_set_alive(board, 1, 0);
    sparse_board_set_alive(board, 0, 1);
    sparse_board_set_alive(board, 1, 1);

    SparseBoard *next = sparse_board_step(board);

    assert(next != NULL);
    assert(sparse_board_population(next) == 4);
    assert(sparse_board_is_alive(next, 0, 0));
    assert(sparse_board_is_alive(next, 1, 0));
    assert(sparse_board_is_alive(next, 0, 1));
    assert(sparse_board_is_alive(next, 1, 1));

    sparse_board_destroy(board);
    sparse_board_destroy(next);
}

static void test_blinker_oscillates(void) {
    SparseBoard *board = sparse_board_create();

    sparse_board_set_alive(board, -1, 0);
    sparse_board_set_alive(board, 0, 0);
    sparse_board_set_alive(board, 1, 0);

    SparseBoard *next = sparse_board_step(board);

    assert(next != NULL);
    assert(sparse_board_population(next) == 3);
    assert(sparse_board_is_alive(next, 0, -1));
    assert(sparse_board_is_alive(next, 0, 0));
    assert(sparse_board_is_alive(next, 0, 1));

    SparseBoard *again = sparse_board_step(next);

    assert(again != NULL);
    assert(sparse_board_population(again) == 3);
    assert(sparse_board_is_alive(again, -1, 0));
    assert(sparse_board_is_alive(again, 0, 0));
    assert(sparse_board_is_alive(again, 1, 0));

    sparse_board_destroy(board);
    sparse_board_destroy(next);
    sparse_board_destroy(again);
}

static void test_glider_moves_without_boundary(void) {
    SparseBoard *board = sparse_board_create();

    sparse_board_set_alive(board, 1, 0);
    sparse_board_set_alive(board, 2, 1);
    sparse_board_set_alive(board, 0, 2);
    sparse_board_set_alive(board, 1, 2);
    sparse_board_set_alive(board, 2, 2);

    SparseBoard *current = board;

    for (int i = 0; i < 4; i++) {
        SparseBoard *next = sparse_board_step(current);
        assert(next != NULL);

        if (current != board) {
            sparse_board_destroy(current);
        }

        current = next;
    }

    assert(sparse_board_population(current) == 5);
    assert(sparse_board_is_alive(current, 2, 1));
    assert(sparse_board_is_alive(current, 3, 2));
    assert(sparse_board_is_alive(current, 1, 3));
    assert(sparse_board_is_alive(current, 2, 3));
    assert(sparse_board_is_alive(current, 3, 3));

    sparse_board_destroy(board);
    sparse_board_destroy(current);
}

int main(void) {
    test_empty_board_has_zero_population();
    test_set_alive_and_dead_with_negative_coordinates();
    test_block_still_life_survives();
    test_blinker_oscillates();
    test_glider_moves_without_boundary();

    puts("All sparse board tests passed.");
    return 0;
}
