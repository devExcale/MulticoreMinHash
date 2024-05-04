#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io_interface.h"
#include "utils.h"

struct Arguments input_arguments(const int argc, const char *argv[]) {

	struct Arguments args = default_arguments();
	const char *help_msg = "Usage: %s "
						   "[--offset <doc_offset>]"
						   "[--shingle <shingle_size>] "
						   "[--signature <signature_size>] "
						   "[--docs <n_docs>] "
						   "[--bandrows <n_band_rows>] "
						   "[--seed <seed>] "
						   "[--verbose <step>] "
						   "[--threshold <threshold>] "
						   "<docs_directory>\n";

	// Check if there are enough arguments
	if (argc < 2) {
		printf(help_msg, argv[0]);
		exit(1);
	}

	int i;
	for (i = 1; i < argc; i++)

		if (strcmp(argv[i], "--offset") == 0)
			args.doc_offset = (unsigned int) atoi(argv[++i]);

		else if (strcmp(argv[i], "--shingle") == 0)
			args.shingle_size = (unsigned int) atoi(argv[++i]);

		else if (strcmp(argv[i], "--signature") == 0)
			args.signature_size = (unsigned int) atoi(argv[++i]);

		else if (strcmp(argv[i], "--docs") == 0)
			args.n_docs = (unsigned int) atoi(argv[++i]);

		else if (strcmp(argv[i], "--bandrows") == 0)
			args.n_band_rows = (unsigned int) atoi(argv[++i]);

		else if (strcmp(argv[i], "--seed") == 0)
			args.seed = atoi(argv[++i]);

		else if (strcmp(argv[i], "--verbose") == 0)
			args.verbose = (unsigned int) atoi(argv[++i]);

		else if (strcmp(argv[i], "--threshold") == 0)
			args.threshold = (float) atof(argv[++i]);

		else {
			args.directory = (char *) argv[i++];
			break;
		}

	// Other arguments after directory
	if (i != argc || !args.directory) {
		printf(help_msg, argv[0]);
		exit(1);
	}

	// Check that bands fill the signature matrix
	if (args.signature_size % args.n_band_rows != 0) {
		printf("The number of rows in a band must be a divisor of the signature size.\n");
		exit(1);
	}

	return args;
}

struct Arguments default_arguments() {

	struct Arguments args;

	args.directory = NULL;
	args.doc_offset = 0;
	args.shingle_size = 3;
	args.signature_size = 100;
	args.n_docs = 0;
	args.n_band_rows = 4;
	args.seed = 13;
	args.verbose = 25;
	args.threshold = .1f;

	return args;
}

void print_arguments(struct Arguments args) {
	printf("-----------------\n");
	printf("[Using arguments]\n");
	printf("- Directory: \"%s\"\n", args.directory);
	printf("- Offset: %u\n", args.doc_offset);
	printf("- Shingle size: %u\n", args.shingle_size);
	printf("- Signature size: %u\n", args.signature_size);
	printf("- Number of documents: %u\n", args.n_docs);
	printf("- Number of rows in bands: %u\n", args.n_band_rows);
	printf("- Seed: %d\n", args.seed);
	printf("- Verbose step: %u\n", args.verbose);
	printf("- Threshold: %.2f\n", args.threshold);
	printf("-----------------\n");
}

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
