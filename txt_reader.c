#include <stdlib.h>
#include <string.h>
#include "txt_reader.h"
#include "utils.h"

char *read_word_from_file(FILE *file) {

	char *buffer = malloc(1024); // 1KB buffer that holds read word
	int words; // Number of characters read

	do {
		// Read word
		words = fscanf(file, "%s", buffer);

		// Handle reading failure (e.g. EOF)
		if (words != 1)
			return NULL;

		// Process word (turn it lowercase and remove non-alphanumeric characters)
		str_tolower_trim_nonalphanum(buffer);

		// Keep reading until a valid word is found (len > 0)
	} while (*buffer == '\0');

	char *ret = strdup(buffer);
	free(buffer);
	return ret;
}

char *read_shingle_from_file(FILE *file, const unsigned int shingle_size, char **prev_words) {

	// Discard oldest word (if NULL free does no-op)
	free(prev_words[0]);

	// Shift words
	for (int i = 1; i < shingle_size; ++i)
		prev_words[i - 1] = prev_words[i];
	prev_words[shingle_size - 1] = NULL;

	// Check if all words are present
	for (int i = 0; i < shingle_size; ++i)
		if (!prev_words[i])
			// Read word if not present
			prev_words[i] = read_word_from_file(file);

	// Error in reading file or EOF, no new shingle of given size
	if (!prev_words[shingle_size - 1])
		return NULL;

	// Compute string length of shingle
	unsigned int shingle_strlen = shingle_size; // (shingle_size - 1) spaces + null terminator
	for (int i = 0; i < shingle_size; ++i)
		shingle_strlen += strlen(prev_words[i]); // + word length

	// Allocate memory for shingle
	char *shingle = malloc(shingle_strlen);

	// Build shingle from words
	unsigned int offset = 0;
	for (int i = 0; i < shingle_size; ++i) {
		strcpy(shingle + offset, prev_words[i]);
		offset += strlen(prev_words[i]);
		shingle[offset++] = ' ';
	}
	shingle[offset - 1] = '\0';

	return shingle;
}