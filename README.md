# Conway's Game of Life in C

A C implementation of John Conway's Game of Life.

The project currently has two simulation paths:

1. A classic terminal version using a fixed-size board.
2. An experimental sparse-board engine that represents an effectively infinite universe by storing only live cells.

The long-term goal is to use the sparse engine as the foundation for a graphical SDL2 renderer and, eventually, larger Game of Life computational structures such as glider-based logic and Turing-machine-style constructions.

## Features

### Classic terminal simulation

- Loads an initial pattern from a text file.
- Runs Conway's Game of Life directly in the terminal.
- Supports interactive controls:
  - `q` quit
  - `+` increase speed
  - `-` decrease speed
  - `p` pause/resume
  - `n` step one generation while paused
  - `r` reset to initial state

### Sparse infinite-board engine

- Stores only live cells.
- Supports negative and positive coordinates.
- Avoids artificial board boundaries.
- Suitable as a foundation for larger Life constructions.
- Includes tests for:
  - empty boards
  - live/dead cell updates
  - negative coordinates
  - still lifes
  - oscillators
  - glider movement without boundary effects

### Sparse viewport demo

- Renders a finite terminal viewport into the sparse infinite board.
- Supports camera movement.
- Supports terminal zoom by changing viewport size.
- Demonstrates the separation between:
  - simulation universe
  - viewport/camera
  - renderer

Sparse demo controls:

- `q` quit
- arrow keys / `WASD` move camera
- `z` zoom in
- `x` zoom out
- `+` increase speed
- `-` decrease speed
- `p` or space pause/resume
- `n` step one generation while paused
- `0` recenter viewport

Mouse-wheel zoom is planned for the future SDL2 renderer rather than the terminal demo.

## Build

```bash
make
```

## Run the classic terminal version

```bash
make run
```

or:

```bash
./build/conway --file patterns/glider.txt
```

With a custom delay:

```bash
./build/conway --file patterns/glider.txt --delay 200
```

With a generation limit:

```bash
./build/conway --file patterns/glider.txt --max-generations 500
```

## Run the sparse viewport demo

```bash
make run-sparse-demo
```

Optional arguments:

```bash
./build/sparse_demo <generations> <delay-ms>
```

Example:

```bash
./build/sparse_demo 300 100
```

Use `-1` generations, or omit the argument, for an open-ended interactive run depending on the current implementation.

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

## Pattern format

The classic terminal simulation loads plain-text pattern files.

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

Example pattern files are stored in:

```text
patterns/
```

## Project structure

```text
include/
  life.h            Classic fixed-board API
  io.h              Pattern loading for classic board
  terminal.h        Terminal input/output helpers
  sparse_board.h    Sparse infinite-board API
  viewport.h        Finite viewport over sparse board

src/
  main.c            Classic terminal application
  life.c            Classic fixed-board rules
  io.c              Classic pattern loader
  terminal.c        Terminal utilities
  sparse_board.c    Sparse infinite-board implementation
  viewport.c        Sparse viewport rendering
  sparse_demo.c     Interactive sparse-board terminal demo

tests/
  test_sparse_board.c
  test_viewport.c

patterns/
  glider.txt
  blinker.txt
```

## Design notes

The classic terminal version is useful for learning, debugging, and preserving a simple runnable baseline.

The sparse engine is the future-facing architecture. Instead of storing every dead cell in a giant grid, it stores only live cells:

```text
(x, y)
```

All other cells are implicitly dead.

This allows the simulation to support coordinates such as:

```text
(-1000, 25)
(0, 0)
(50000, -900)
```

The viewport decides which finite region of the infinite universe is visible.

This separation is important for future graphical rendering:

```text
SparseBoard -> Viewport/Camera -> Renderer
```

## Roadmap

Planned milestones:

- Keep the classic terminal version as a baseline.
- Improve sparse pattern loading from files.
- Add configurable pattern placement.
- Add SDL2 renderer.
- Add mouse-wheel zoom in SDL2.
- Add screenshot and frame export.
- Add video generation through exported frames and `ffmpeg`.
- Add support for standard Life RLE pattern files.
- Add larger known Life patterns:
  - glider gun
  - eaters
  - reflectors
  - oscillators
- Explore glider-based logic gates.
- Build toward a Turing-machine-style Game of Life construction.

## Video export idea

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

## License

See the repository license file.
