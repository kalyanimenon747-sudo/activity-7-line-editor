# Activity 7: Simple Line Editor in C

**Course:** B25GE0101 (3rd Semester)  
**Team Members:** <Team Member Names>

## Data Structure
Dynamic array of heap-allocated strings (`char **lines`, `int count`, `int capacity`) grown via `realloc`. It gives fast indexed access when printing lines and avoids complex linked-list pointer manipulation while keeping memory management straightforward.

## Implemented Features
- **Core:** Insert a line (`i`), delete a line (`d`), display document with line numbers (`p`), save/load `.txt` files (`s`, `l`).
- **Bonus:** Search word/phrase (`f`), line and word count (`c`).

## Compile and Run
```bash
gcc -Wall -Wextra -std=c11 editor.c -o editor
./editor
