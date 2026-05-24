CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Iinclude
LDFLAGS :=
SDL_CFLAGS := $(shell sdl2-config --cflags 2>/dev/null)
SDL_LDFLAGS := $(shell sdl2-config --libs 2>/dev/null)

SRC := src/main.c src/life.c src/io.c src/terminal.c
OBJ := $(SRC:src/%.c=build/%.o)
BIN := build/conway

.PHONY: all clean run debug

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(BIN) $(LDFLAGS)

build/%.o: src/%.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(BIN) --file patterns/glider.txt

debug: CFLAGS += -g -O0
debug: clean all

clean:
	rm -rf build/*.o $(BIN)


.PHONY: test-sparse

test-sparse: build/test_sparse_board
	./build/test_sparse_board

build/test_sparse_board: tests/test_sparse_board.c src/sparse_board.c include/sparse_board.h
	@mkdir -p build
	$(CC) $(CFLAGS) tests/test_sparse_board.c src/sparse_board.c -o build/test_sparse_board


.PHONY: test-viewport run-sparse-demo

test-viewport: build/test_viewport
	./build/test_viewport

build/test_viewport: tests/test_viewport.c src/viewport.c src/sparse_board.c include/viewport.h include/sparse_board.h
	@mkdir -p build
	$(CC) $(CFLAGS) tests/test_viewport.c src/viewport.c src/sparse_board.c -o build/test_viewport

build/sparse_demo: src/sparse_demo.c src/viewport.c src/sparse_board.c include/viewport.h include/sparse_board.h
	@mkdir -p build
	$(CC) $(CFLAGS) src/sparse_demo.c src/viewport.c src/sparse_board.c -o build/sparse_demo

run-sparse-demo: build/sparse_demo
	./build/sparse_demo


.PHONY: run-sdl

build/conway_sdl: src/sdl_main.c src/renderer_sdl.c src/sparse_board.c src/viewport.c include/renderer_sdl.h include/sparse_board.h include/viewport.h
	@mkdir -p build
	$(CC) $(CFLAGS) $(SDL_CFLAGS) src/sdl_main.c src/renderer_sdl.c src/sparse_board.c src/viewport.c -o build/conway_sdl $(SDL_LDFLAGS)

run-sdl: build/conway_sdl
	./build/conway_sdl
