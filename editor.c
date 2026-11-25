#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT_LEN 1024

typedef struct {
    char **lines;
    int count;
    int capacity;
} Document;

static void init_doc(Document *doc) {
    doc->lines = NULL;
    doc->count = 0;
    doc->capacity = 0;
}

static void free_doc(Document *doc) {
    for (int i = 0; i < doc->count; i++) {
        free(doc->lines[i]);
    }
    free(doc->lines);
    doc->lines = NULL;
    doc->count = 0;
    doc->capacity = 0;
}

static int ensure_capacity(Document *doc, int needed) {
    if (needed <= doc->capacity) {
        return 1;
    }
    int new_cap = (doc->capacity == 0) ? 8 : doc->capacity;
    while (new_cap < needed) {
        new_cap *= 2;
    }
    char **resized = (char **)realloc(doc->lines, (size_t)new_cap * sizeof(char *));
    if (!resized) {
        return 0;
    }
    doc->lines = resized;
    doc->capacity = new_cap;
    return 1;
}

static char *dup_string(const char *src) {
    size_t len = strlen(src);
    char *copy = (char *)malloc(len + 1);
    if (!copy) {
        return NULL;
    }
    memcpy(copy, src, len + 1);
    return copy;
}

/* CORE 1: Insert text at 1-based line number `pos` (1 <= pos <= count + 1) */
static int insert_line(Document *doc, int pos, const char *text) {
    if (pos < 1 || pos > doc->count + 1) {
        printf("Error: Line number must be between 1 and %d.\n", doc->count + 1);
        return 0;
    }
    if (!ensure_capacity(doc, doc->count + 1)) {
        printf("Error: Out of memory while inserting line.\n");
        return 0;
    }
    char *copy = dup_string(text);
    if (!copy) {
        printf("Error: Out of memory while copying line text.\n");
        return 0;
    }

    for (int i = doc->count; i >= pos; i--) {
        doc->lines[i] = doc->lines[i - 1];
    }
    doc->lines[pos - 1] = copy;
    doc->count++;
    return 1;
}

/* CORE 2: Delete line at 1-based line number `pos` (1 <= pos <= count) */
static int delete_line(Document *doc, int pos) {
    if (doc->count == 0) {
        printf("Error: Document is empty; nothing to delete.\n");
        return 0;
    }
    if (pos < 1 || pos > doc->count) {
        printf("Error: Line number must be between 1 and %d.\n", doc->count);
        return 0;
    }

    free(doc->lines[pos - 1]);
    for (int i = pos - 1; i < doc->count - 1; i++) {
        doc->lines[i] = doc->lines[i + 1];
    }
    doc->count--;
    if (doc->count == 0) {
        free(doc->lines);
        doc->lines = NULL;
        doc->capacity = 0;
    }
    return 1;
}

/* CORE 3: Display all lines with 1-based line numbers */
static void display_doc(const Document *doc) {
    if (doc->count == 0) {
        printf("[Document is empty]\n");
        return;
    }
    for (int i = 0; i < doc->count; i++) {
        printf("%d: %s\n", i + 1, doc->lines[i]);
    }
}

/* CORE 4a: Save document to a .txt file */
static int save_file(const Document *doc, const char *filename) {
    if (!filename || filename[0] == '\0') {
        printf("Error: Please provide a filename (e.g., s notes.txt).\n");
        return 0;
    }
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("Error: Could not open '%s' for writing.\n", filename);
        return 0;
    }
    for (int i = 0; i < doc->count; i++) {
        fprintf(fp, "%s\n", doc->lines[i]);
    }
    fclose(fp);
    printf("Saved %d line(s) to '%s'.\n", doc->count, filename);
    return 1;
}

/* CORE 4b: Load lines from a .txt file (replaces current in-memory lines) */
static int load_file(Document *doc, const char *filename) {
    if (!filename || filename[0] == '\0') {
        printf("Error: Please provide a filename (e.g., l notes.txt).\n");
        return 0;
    }
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Error: Could not open '%s' for reading.\n", filename);
        return 0;
    }

    Document loaded;
    init_doc(&loaded);

    char buffer[MAX_INPUT_LEN];
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        size_t len = strlen(buffer);
        while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
            buffer[--len] = '\0';
        }
        if (!insert_line(&loaded, loaded.count + 1, buffer)) {
            fclose(fp);
            free_doc(&loaded);
            printf("Error: Failed while loading '%s'.\n", filename);
            return 0;
        }
    }

    fclose(fp);
    free_doc(doc);
    *doc = loaded;
    printf("Loaded %d line(s) from '%s'.\n", doc->count, filename);
    return 1;
}

/* BONUS: Search for word or phrase across lines */
static void search_doc(const Document *doc, const char *query) {
    if (!query || query[0] == '\0') {
        printf("Error: Please provide a word or phrase to search (e.g., f todo).\n");
        return;
    }
    if (doc->count == 0) {
        printf("[Document is empty]\n");
        return;
    }
    int matches = 0;
    for (int i = 0; i < doc->count; i++) {
        if (strstr(doc->lines[i], query) != NULL) {
            printf("Line %d: %s\n", i + 1, doc->lines[i]);
            matches++;
        }
    }
    if (matches == 0) {
        printf("No lines found matching \"%s\".\n", query);
    }
}

/* BONUS: Count total lines and words */
static void show_stats(const Document *doc) {
    long words = 0;
    for (int i = 0; i < doc->count; i++) {
        int in_word = 0;
        for (const char *p = doc->lines[i]; *p != '\0'; p++) {
            if (isspace((unsigned char)*p)) {
                in_word = 0;
            } else if (!in_word) {
                in_word = 1;
                words++;
            }
        }
    }
    printf("Lines: %d | Words: %ld\n", doc->count, words);
}

static void print_help(void) {
    printf("Commands:\n");
    printf("  i <line> <text>  Insert <text> at 1-based <line>\n");
    printf("  d <line>         Delete 1-based <line>\n");
    printf("  p                Display all lines with line numbers\n");
    printf("  s <filename>     Save document to <filename>\n");
    printf("  l <filename>     Load document from <filename>\n");
    printf("  f <query>        Search lines containing <query>\n");
    printf("  c                Show line count and word count\n");
    printf("  h                Show this help message\n");
    printf("  q                Quit the editor\n");
}

static void trim_trailing_newline(char *s) {
    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[--len] = '\0';
    }
}

static char *skip_spaces(char *p) {
    while (*p != '\0' && isspace((unsigned char)*p)) {
        p++;
    }
    return p;
}

int main(int argc, char **argv) {
    Document doc;
    init_doc(&doc);

    if (argc >= 2) {
        load_file(&doc, argv[1]);
    }

    printf("Simple C Line Editor (type 'h' for help, 'q' to quit)\n");

    char input[MAX_INPUT_LEN];
    while (1) {
        printf("> ");
        fflush(stdout);
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");
            break;
        }
        trim_trailing_newline(input);
        char *line = skip_spaces(input);
        if (*line == '\0') {
            continue;
        }

        char cmd = line[0];
        char *rest = skip_spaces(line + 1);

        if (cmd == 'q') {
            break;
        } else if (cmd == 'h') {
            print_help();
        } else if (cmd == 'p') {
            display_doc(&doc);
        } else if (cmd == 'c') {
            show_stats(&doc);
        } else if (cmd == 'i') {
            char *endptr = NULL;
            long pos = strtol(rest, &endptr, 10);
            if (endptr == rest || (*endptr != '\0' && !isspace((unsigned char)*endptr))) {
                printf("Error: Usage: i <line_number> <text>\n");
                continue;
            }
            char *text = skip_spaces(endptr);
            insert_line(&doc, (int)pos, text);
        } else if (cmd == 'd') {
            if (*rest == '\0') {
                printf("Error: Usage: d <line_number>\n");
                continue;
            }
            char *endptr = NULL;
            long pos = strtol(rest, &endptr, 10);
            endptr = skip_spaces(endptr);
            if (endptr == rest || *endptr != '\0') {
                printf("Error: Usage: d <line_number>\n");
                continue;
            }
            delete_line(&doc, (int)pos);
        } else if (cmd == 's') {
            char *end = rest + strlen(rest);
            while (end > rest && isspace((unsigned char)*(end - 1))) {
                *(--end) = '\0';
            }
            save_file(&doc, rest);
        } else if (cmd == 'l') {
            char *end = rest + strlen(rest);
            while (end > rest && isspace((unsigned char)*(end - 1))) {
                *(--end) = '\0';
            }
            load_file(&doc, rest);
        } else if (cmd == 'f') {
            char *end = rest + strlen(rest);
            while (end > rest && isspace((unsigned char)*(end - 1))) {
                *(--end) = '\0';
            }
            search_doc(&doc, rest);
        } else {
            printf("Unknown command '%c'. Type 'h' for help.\n", cmd);
        }
    }

    free_doc(&doc);
    return 0;
}