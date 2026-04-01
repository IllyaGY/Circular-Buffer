# Generic Circular Buffer in C

A small C project that demonstrates two generic circular-buffer-style implementations:

- **Array-based circular buffer**
- **Linked-list-based circular queue**

The program uses **ncurses** for terminal visualization and input.

---

## Features

- Generic storage using `void *` + `type_size`
- Array-based circular buffer implementation
- Linked-list-based circular implementation
- `ncurses` UI for displaying contents live
- Compile-time switching between implementations
- Compile-time test-case presets
- Compile-time size selection

---

## Requirements

- C compiler (`gcc` or `clang`)
- `ncurses`

---

## File Overview

- `run.c` — main program and UI
- `buffer_api.c` / `buffer_api.h` — wrapper API
- `buffer_array.c` / `buffer_array.h` — array implementation
- `buffer_list.c` / `buffer_list.h` — linked-list implementation

---

## Compile-Time Options

This project uses macros passed through the compiler command line.

### `TYPE`
Selects which implementation to build:

- `TYPE=1` → array version
- `TYPE=0` → linked-list version

### `TEST_CASE`
Selects initial fill state:

- `TEST_CASE=0` → fully filled buffer
- `TEST_CASE=1` → half-filled buffer
- any other value → empty buffer

### `SIZE`
Sets the buffer capacity.

Example:

```bash
-DSIZE=10
-DTYPE=0
-DTEST_CASE=0
```

---

## Build and Run

### Array Version

```bash
gcc -DTYPE=1 -DTEST_CASE=0 -DSIZE=10 -g run.c buffer_api.c buffer_array.c buffer_list.c -o array.out -lncurses
./array.out
```

### Linked List Version

```bash
gcc -DTYPE=0 -DTEST_CASE=0 -DSIZE=10 -g run.c buffer_api.c buffer_array.c buffer_list.c -o list.out -lncurses
./list.out
```

---

## Example Builds

### Array Implementation

#### Full buffer
```bash
gcc -DTYPE=1 -DTEST_CASE=0 -DSIZE=10 -g run.c buffer_api.c buffer_array.c buffer_list.c -o array_full.out -lncurses
./array_full.out
```

#### Half-filled buffer
```bash
gcc -DTYPE=1 -DTEST_CASE=1 -DSIZE=10 -g run.c buffer_api.c buffer_array.c buffer_list.c -o array_half.out -lncurses
./array_half.out
```

#### Empty buffer
```bash
gcc -DTYPE=1 -DTEST_CASE=2 -DSIZE=10 -g run.c buffer_api.c buffer_array.c buffer_list.c -o array_empty.out -lncurses
./array_empty.out
```

### Linked-List Implementation

#### Full buffer
```bash
gcc -DTYPE=0 -DTEST_CASE=0 -DSIZE=10 -g run.c buffer_api.c buffer_array.c buffer_list.c -o list_full.out -lncurses
./list_full.out
```

#### Half-filled buffer
```bash
gcc -DTYPE=0 -DTEST_CASE=1 -DSIZE=10 -g run.c buffer_api.c buffer_array.c buffer_list.c -o list_half.out -lncurses
./list_half.out
```

#### Empty buffer
```bash
gcc -DTYPE=0 -DTEST_CASE=2 -DSIZE=10 -g run.c buffer_api.c buffer_array.c buffer_list.c -o list_empty.out -lncurses
./list_empty.out
```

---

## Controls

While the program is running:

- `i` — push an element
- `r` — pop an element
- `d` — destroy the buffer

---

## Implementation Notes

### Array Version
The array implementation uses:

- `buf` — raw storage
- `type_size` — size of one stored item
- `head` — index of the oldest element
- `tail` — index of the next insertion slot
- `count` — number of valid elements
- `capacity` / `size` — total buffer capacity


### Linked-List Version
The linked-list version stores each item in a node:

- each node owns its own allocated item storage
- the list tracks `head`, `tail`, `count`, and `capacity`
- display order is logical: head at the top, tail below it
- empty slots are shown below the tail


---

## Notes About Generic Storage

This project stores arbitrary objects using:

- `void *`
- `size_t type_size`
- `memcpy`

For array-based raw memory access, pointer arithmetic is done with `char *`, for example:

```c
(char *)buf->buf + index * buf->type_size
```
