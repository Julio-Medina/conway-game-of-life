# Conway's Game of Life in C

A C implementation of John Conway's Game of Life with multiple frontends:

1. A classic terminal version using a fixed-size board.
2. A sparse infinite-board engine with a terminal viewport demo.
3. A graphical SDL2 renderer built on top of the sparse-board engine.

The long-term goal is to use the sparse engine and graphical renderer as the foundation for larger Game of Life constructions, including glider-based logic and eventually Turing-machine-style experiments.

## What is Conway's Game of Life?

Conway's Game of Life is a cellular automaton played on a grid of cells. Each cell is either alive or dead. Every generation is computed from the previous one using four rules:

1. A live cell with fewer than two live neighbors dies.
2. A live cell with two or three live neighbors survives.
3. A live cell with more than three live neighbors dies.
4. A dead cell with exactly three live neighbors becomes alive.

Simple rules can produce complex behavior such as oscillators, spaceships, logic gates, memory, and computation.

## Project modes

### 1. Classic terminal version

The original version uses a fixed-size terminal board.

It is useful as:

- a simple baseline implementation
- a fast way to test pattern loading
- a terminal-only demo
- a clean first implementation of the Game of Life rules

Run it with:

```bash
make run
```

or directly:

```bash
./build/conway --file patterns/glider.txt
```

Useful options:

```bash
./build/conway --file patterns/glider.txt --delay 200
./build/conway --file patterns/glider.txt --max-generations 500
./build/conway --file patterns/blinker.txt --delay 500
```

Classic terminal controls:

| Key | Action |
|---|---|
| `q` | quit |
| `+` | increase speed |
| `-` | decrease speed |
| `p` | pause/resume |
| `n` | step one generation while paused |
| `r` | reset to initial state |

### 2. Sparse infinite-board engine

The sparse engine represents an effectively infinite Game of Life board.

Instead of storing every cell in a fixed array, it stores only live cells:

```text
(x, y)
```

All other cells are implicitly dead.

This means the board can contain coordinates like:

```text
(-1000, 25)
(0, 0)
(50000, -900)
```

This avoids artificial boundaries and is the correct foundation for larger Life constructions.

The sparse board is implemented in:

```text
include/sparse_board.h
src/sparse_board.c
```

The main sparse API includes:

```c
SparseBoard *sparse_board_create(void);
void sparse_board_destroy(SparseBoard *board);

int sparse_board_is_alive(const SparseBoard *board, int x, int y);
int sparse_board_set_alive(SparseBoard *board, int x, int y);
int sparse_board_set_dead(SparseBoard *board, int x, int y);

SparseBoard *sparse_board_step(const SparseBoard *current);
size_t sparse_board_population(const SparseBoard *board);
```

### 3. Sparse terminal viewport demo

The sparse terminal demo renders a finite viewport into the sparse infinite board.

This separates:

```text
SparseBoard simulation universe
        ↓
Viewport / camera
        ↓
Terminal renderer
```

Run it with:

```bash
make run-sparse-demo
```

or directly:

```bash
./build/sparse_demo
```

Optional arguments:

```bash
./build/sparse_demo <generations> <delay-ms>
```

Example:

```bash
./build/sparse_demo 300 100
```

Sparse terminal controls:

| Key | Action |
|---|---|
| `q` | quit |
| arrow keys / `WASD` | move camera |
| `z` | zoom in by shrinking the terminal viewport |
| `x` | zoom out by expanding the terminal viewport |
| `+` | increase speed |
| `-` | decrease speed |
| `p` or space | pause/resume |
| `n` | step one generation while paused |
| `0` | recenter viewport |

Note: mouse-wheel zoom is intentionally handled in the SDL2 renderer, not the terminal demo.

### 4. SDL2 graphical renderer

The SDL2 version uses the sparse infinite-board engine and renders it in a graphical window.

This is the preferred visual frontend for larger experiments.

It supports:

- graphical rendering
- resizable window
- camera movement
- mouse-wheel zoom
- keyboard zoom
- speed control
- pause/resume
- generation and population display in the window title

The SDL2 renderer is implemented in:

```text
include/renderer_sdl.h
src/renderer_sdl.c
src/sdl_main.c
```

Run it with:

```bash
make run-sdl
```

or directly:

```bash
./build/conway_sdl
```

SDL2 controls:

| Key / Input | Action |
|---|---|
| `q` / Esc | quit |
| Space / `p` | pause/resume |
| `n` | step once while paused |
| arrow keys / `WASD` | move camera |
| mouse wheel | zoom |
| `z` | zoom in |
| `x` | zoom out |
| `+` | increase speed |
| `-` | decrease speed |
| `0` | recenter camera |

## Dependencies

### Required

- GCC or another C compiler
- Make

On Debian/Ubuntu:

```bash
sudo apt update
sudo apt install build-essential
```

### Optional: SDL2 graphical renderer

To build and run the SDL2 version:

```bash
sudo apt update
sudo apt install libsdl2-dev
```

Verify SDL2 is available:

```bash
sdl2-config --version
```

If `sdl2-config` is not found, install `libsdl2-dev`.

## Build

Build the classic terminal executable:

```bash
make
```

This creates:

```text
build/conway
```

Build and run the sparse terminal demo:

```bash
make run-sparse-demo
```

This creates:

```text
build/sparse_demo
```

Build and run the SDL2 renderer:

```bash
make run-sdl
```

This creates:

```text
build/conway_sdl
```

## Tests

Run sparse-board tests:

```bash
make test-sparse
```

Run viewport tests:

```bash
make test-viewport
```

Recommended before committing:

```bash
make test-sparse
make test-viewport
```

The tests currently cover:

- empty sparse board behavior
- setting live and dead cells
- negative coordinates
- still-life behavior
- oscillator behavior
- glider movement without boundary effects
- viewport containment
- viewport movement and centering
- sparse board rendering into a viewport string

## Pattern files

The classic terminal version loads plain-text pattern files.

Example:

```text
.O.
..O
OOO
```

Alive cells can be represented with:

```text
O
o
1
#
```

Dead cells can be represented with:

```text
.
space
```

Example files are stored in:

```text
patterns/
```

Current examples:

```text
patterns/glider.txt
patterns/blinker.txt
```

## Project structure

```text
include/
  life.h              Classic fixed-board API
  io.h                Pattern loading for the classic board
  terminal.h          Terminal input/output helpers
  sparse_board.h      Sparse infinite-board API
  viewport.h          Finite viewport over sparse board
  renderer_sdl.h      SDL2 renderer API

src/
  main.c              Classic terminal application
  life.c              Classic fixed-board rules
  io.c                Classic pattern loader
  terminal.c          Terminal utilities
  sparse_board.c      Sparse infinite-board implementation
  viewport.c          Sparse viewport rendering
  sparse_demo.c       Interactive sparse-board terminal demo
  renderer_sdl.c      SDL2 drawing implementation
  sdl_main.c          SDL2 application entry point

tests/
  test_sparse_board.c
  test_viewport.c

patterns/
  glider.txt
  blinker.txt

build/
  generated binaries and object files
```

## Design overview

The project intentionally keeps simulation and rendering separate.

The classic version is simple:

```text
Fixed Board -> Terminal Renderer
```

The newer architecture is more flexible:

```text
SparseBoard -> Viewport/Camera -> Renderer
```

This allows the same simulation engine to be rendered in different ways:

```text
SparseBoard -> Terminal viewport
SparseBoard -> SDL2 window
SparseBoard -> future frame/video exporter
```

This separation is important because the final goal is not just animation. The goal is to support large Life structures where the board must be effectively unbounded.

## Why sparse boards?

A fixed board has artificial boundaries.

A toroidal board wraps edges around:

```text
right edge -> left edge
top edge   -> bottom edge
```

That can be visually interesting, but it is not appropriate for serious Life computation because signals can interact through the wrapped boundary.

The sparse board avoids both problems:

- no hard boundary
- no artificial wrapping
- only live cells use memory

This is the correct direction for gliders, glider guns, logic gates, and Turing-machine-style constructions.

## Current roadmap

Planned milestones:

- Keep the classic terminal version as a baseline.
- Improve sparse pattern loading from files.
- Add configurable pattern placement.
- Improve SDL2 rendering and camera behavior.
- Add mouse-centered zoom.
- Add screenshot export.
- Add frame export.
- Generate videos from exported frames using `ffmpeg`.
- Add support for standard Life RLE pattern files.
- Add larger known Life patterns:
  - glider gun
  - eaters
  - reflectors
  - oscillators
- Explore glider-based logic gates.
- Build toward a Turing-machine-style Game of Life construction.

## Future video export idea

A future version may export frames:

```text
frames/frame_000001.pgm
frames/frame_000002.pgm
frames/frame_000003.pgm
```

Then create a video with:

```bash
ffmpeg -framerate 30 -i frames/frame_%06d.pgm -pix_fmt yuv420p conway.mp4
```

## Git workflow used in this project

Recommended workflow:

```bash
git checkout main
git pull
git checkout -b feature/name-of-feature
```

After changes:

```bash
make test-sparse
make test-viewport
```

Then:

```bash
git add .
git commit -m "Describe the feature"
git push -u origin feature/name-of-feature
```

Open a pull request into `main`.

## License

See the repository license file.
