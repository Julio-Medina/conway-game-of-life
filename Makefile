CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Iinclude
LDFLAGS :=

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
