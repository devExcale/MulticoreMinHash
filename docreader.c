#ifndef DOCREADER_C
#define DOCREADER_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.c"

/// \brief Read a document from a file.
char **read_document(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return NULL;
    }

    char **lines = NULL;
    int line_count = 0;
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        char *line = strdup(buffer);
        lines = realloc(lines, (line_count + 1) * sizeof(char *));
        lines[line_count++] = line;
    }

    fclose(file);
    lines = realloc(lines, (line_count + 1) * sizeof(char *));
    lines[line_count] = NULL;

    return lines;
}

/// \brief Read a word from a file.
/// The returned string's memory must be freed by the caller.
/// \param file File to read from
/// \return The word read from the file
char *read_word_from_file(FILE *file) {
    char *buffer = malloc(1024); // 1KB buffer that holds read word
    int words; // Number of characters read

    do {
        // Read word
        words = fscanf(file, "%s", buffer);

        // Handle reading failure
        if (words != 1)
            return NULL;

        // printf("Readed word: %s\n", buffer);

        // Process word
        str_tolower_trim_nonalphanum(buffer);

        // Keep reading until a valid word is found
    } while (*buffer == '\0');

    char *ret = strdup(buffer);
    free(buffer);
    return ret;
}

/// Returns a string containing a shingle read from a given file. <br><br>
/// The returned string's memory must be freed by the caller. <br>
/// Once no more shingles are to be read, the caller must free the memory used by prev_words.
/// \param file File to read from
/// \param n_words How many words in the shingle
/// \param prev_words An array of words previously read
/// \return The read shingle
char *read_shingle_from_file(FILE *file, int n_words, char **prev_words) {

    // Discard oldest word (if NULL free does no-op)
    free(prev_words[0]);

    // Shift words
    for (int i = 1; i < n_words; ++i)
        prev_words[i - 1] = prev_words[i];
    prev_words[n_words - 1] = NULL;

    // Read words
    for (int i = 0; i < n_words; ++i)
        // Read word if not present
        if (!prev_words[i])
            prev_words[i] = read_word_from_file(file);

    // Error in reading file or EOF, no new shingle of given size
    if (!prev_words[n_words - 1])
        return NULL;

    // Compute string length of shingle

    int len = n_words; // (n_words - 1) spaces + null terminator
    for (int i = 0; i < n_words; ++i)
        len += strlen(prev_words[i]); // Word length

    // Allocate memory for shingle
    char *shingle = malloc(len);

    // Copy words to shingle
    int offset = 0;
    for (int i = 0; i < n_words; ++i) {
        strcpy(shingle + offset, prev_words[i]);
        offset += strlen(prev_words[i]);
        shingle[offset++] = ' ';
    }
    shingle[offset - 1] = '\0';

    return shingle;
}

#endif
