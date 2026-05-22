#include "sparse_board.h"
#include "viewport.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static void test_viewport_contains_boundaries(void) {
    Viewport viewport = viewport_create(-2, -1, 4, 3);

    assert(viewport_contains(viewport, -2, -1));
    assert(viewport_contains(viewport, 1, 1));
    assert(!viewport_contains(viewport, 2, 1));
    assert(!viewport_contains(viewport, 1, 2));
    assert(!viewport_contains(viewport, -3, 0));
    assert(!viewport_contains(viewport, 0, -2));
}

static void test_viewport_move_and_center(void) {
    Viewport viewport = viewport_create(0, 0, 10, 6);

    viewport_move(&viewport, 5, -3);
    assert(viewport.origin_x == 5);
    assert(viewport.origin_y == -3);

    viewport_center_on(&viewport, 10, 10);
    assert(viewport.origin_x == 5);
    assert(viewport.origin_y == 7);
}

static void test_render_sparse_board_to_string(void) {
    SparseBoard *board = sparse_board_create();
    assert(board != NULL);

    sparse_board_set_alive(board, 0, 0);
    sparse_board_set_alive(board, 2, 1);
    sparse_board_set_alive(board, -1, -1);

    Viewport viewport = viewport_create(-1, -1, 4, 3);
    char *rendered = viewport_render_to_string(board, viewport);

    assert(rendered != NULL);
    assert(strcmp(rendered, "O...\n.O..\n...O\n") == 0);

    free(rendered);
    sparse_board_destroy(board);
}

int main(void) {
    test_viewport_contains_boundaries();
    test_viewport_move_and_center();
    test_render_sparse_board_to_string();
    return 0;
}
