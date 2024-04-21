#include <stdlib.h>
#include <string.h>
#include "txt_reader.h"
#include "utils.h"

char *read_word_from_file(FILE *p_file) {

	char *buffer = malloc(1024); // 1KB buffer that holds read word
	int words; // Number of characters read

	do {
		// Read word
		words = fscanf(p_file, "%s", buffer);

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

char *read_shingle_from_file(FILE *file, const unsigned int shingle_size, char **p_words) {

	// Discard oldest word (if NULL free does no-op)
	free(p_words[0]);

	// Shift words
	for (int i = 1; i < shingle_size; ++i)
		p_words[i - 1] = p_words[i];
	p_words[shingle_size - 1] = NULL;

	// Check if all words are present
	for (int i = 0; i < shingle_size; ++i)
		if (!p_words[i])
			// Read word if not present
			p_words[i] = read_word_from_file(file);

	// Error in reading file or EOF, no new shingle of given size
	if (!p_words[shingle_size - 1])
		return NULL;

	// Compute string length of shingle
	unsigned int shingle_strlen = shingle_size; // (shingle_size - 1) spaces + null terminator
	for (int i = 0; i < shingle_size; ++i)
		shingle_strlen += strlen(p_words[i]); // + word length

	// Allocate memory for shingle
	char *shingle = malloc(shingle_strlen);

	// Build shingle from words
	unsigned int offset = 0;
	for (int i = 0; i < shingle_size; ++i) {
		strcpy(shingle + offset, p_words[i]);
		offset += strlen(p_words[i]);
		shingle[offset++] = ' ';
	}
	shingle[offset - 1] = '\0';

	return shingle;
}