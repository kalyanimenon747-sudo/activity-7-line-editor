# Line Editor Help (`HELP.md`)

All line numbers are **1-based** (`1` is the first line).

## Commands

### `i <line> <text>` — Insert a line
Inserts `<text>` at line `<line>`, shifting existing lines down.
- **Example:** `i 1 First line of text`

### `d <line>` — Delete a line
Deletes line `<line>`, shifting lines below it up.
- **Example:** `d 1`

### `p` — Display the document
Prints all current lines prefixed with their line numbers.
- **Example:** `p`

### `s <filename>` — Save document to file
Writes all current lines to `<filename>`.
- **Example:** `s notes.txt`

### `l <filename>` — Load document from file
Replaces the current buffer with lines read from `<filename>`. You can also pass a filename on startup: `./editor notes.txt`.
- **Example:** `l notes.txt`

### `f <word_or_phrase>` — Search (Bonus)
Finds and prints every line containing `<word_or_phrase>`.
- **Example:** `f todo`

### `c` — Line & word count (Bonus)
Displays the total number of lines and words in the document.
- **Example:** `c`

### `h` — Show help
Prints the command list.
- **Example:** `h`

### `q` — Quit
Frees memory and exits the editor.
- **Example:** `q`