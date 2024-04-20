#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "txt_reader.h"
#include "main.h"
#include "utils.h"

struct Arguments {
	// Directory where to pull the documents from
	char *directory;
	// How many words in a shingle
	unsigned int shingle_size;
	// Number of hashes to compute for a document
	unsigned int signature_size;
	// Number of documents to process
	unsigned int n_docs;
	// Number of rows in each band
	unsigned int n_band_rows;
	// Hash function seed
	int seed;
	// Whether to print verbose information
	bool verbose;
	// After how many steps to print verbose information
	unsigned int verbose_step;
};

int main(const int argc, const char *argv[]) {

	// Get arguments
	struct Arguments args = input_arguments(argc, argv);

	if (args.verbose)
		print_arguments(args);

	main_min_hash(args);

	return 0;
}

struct Arguments input_arguments(const int argc, const char *argv[]) {

	struct Arguments args = default_arguments();
	const char *help_msg = "Usage: %s "
						   "[--shingle <shingle_size>] "
						   "[--signature <signature_size>] "
						   "[--docs <n_docs>] "
						   "[--bandrows <n_band_rows>] "
						   "[--seed <seed>] "
						   "[--verbose] "
						   "[--verstep <verbose_step>] "
						   "<doc_directory>\n";

	// Check if there are enough arguments
	if (argc < 2) {
		printf(help_msg, argv[0]);
		exit(1);
	}

	int i;
	for (i = 1; i < argc; i++)

		if (strcmp(argv[i], "--shingle") == 0)
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
			args.verbose = true;

		else if (strcmp(argv[i], "--verstep") == 0)
			args.verbose_step = (unsigned int) atoi(argv[++i]);

		else {
			args.directory = (char *) argv[i];
			break;
		}

	// Other arguments after directory
	if (!args.directory) {
		printf(help_msg, argv[0]);
		exit(1);
	}

	// Check number of documents and rows in bands
	if (args.n_docs % args.n_band_rows != 0) {
		printf("The number of rows in a band must be a divisor of the number of documents.\n");
		exit(1);
	}

	return args;
}

struct Arguments default_arguments() {

	struct Arguments args;

	args.directory = NULL;
	args.shingle_size = 3;
	args.signature_size = 100;
	args.n_docs = 0;
	args.n_band_rows = 4;
	args.seed = 13;
	args.verbose = false;
	args.verbose_step = 25;

	return args;
}

void print_arguments(struct Arguments args) {
	printf("-----\n");
	printf("[Using arguments]\n");
	printf("- Directory: \"%s\"\n", args.directory);
	printf("- Shingle size: %u\n", args.shingle_size);
	printf("- Signature size: %u\n", args.signature_size);
	printf("- Number of documents: %u\n", args.n_docs);
	printf("- Number of rows in bands: %u\n", args.n_band_rows);
	printf("- Seed: %d\n", args.seed);
	printf("- Verbose: %s\n", args.verbose ? "true" : "false");
	printf("- Verbose step: %u\n", args.verbose_step);
	printf("-----\n");
}

int main_min_hash(struct Arguments args) {

	const int n_hashes = args.signature_size;
	const int shingle_size = args.shingle_size;
	const int n_docs = args.n_docs;

	uint32_t signature_matrix[n_docs][n_hashes];
	memset(signature_matrix, 0, n_docs * n_hashes * sizeof(uint32_t)); // Void signature matrix

	const int n_band_rows = args.n_band_rows;
	const int n_bands = n_hashes / n_band_rows;
	uint32_t bands_matrix[n_docs][n_bands];
	memset(bands_matrix, 0, n_docs * n_bands * sizeof(uint32_t)); // Void bands matrix

	const int offset = 1;

	// Compute signature matrix
	for (int i = 0; i < n_docs; ++i) {

		if (args.verbose && (i % args.verbose_step == 0 || i == n_docs - 1))
			printf("Computing signature for doc %d\n", i + offset);

		// Compute the path of the document file (they are numbered)
		char *doc_filepath = (char *) malloc((strlen(args.directory) + 10) * sizeof(char));
		sprintf(doc_filepath, "%s\\%d.txt", args.directory, i + offset);

		// Compute signature hashes
		for (int j = 0; j < n_hashes; ++j)
			signature_matrix[i][j] = min_hash_shingle(doc_filepath, shingle_size, j * args.seed);

		// Free file path memory
		free(doc_filepath);
	}

	// Compute bands hashes
	for (int i = 0; i < n_docs; ++i) {

		if (args.verbose && (i % args.verbose_step == 0 || i == n_docs - 1))
			printf("Computing bands for doc %d\n", i + offset);

		for (int j = 0; j < n_bands; ++j) {

			uint32_t band_hash = 0;

			for (int k = 0; k < n_band_rows; ++k)
				band_hash ^= signature_matrix[i][j * n_band_rows + k];

			bands_matrix[i][j] = band_hash;
		}
	}

	printf("Starting comparison...\n");

	// Compare all pairs of documents
	for (int i = 0; i < n_docs - 1; ++i)
		for (int j = i + 1; j < n_docs; ++j) {

			if (!is_candidate_pair(bands_matrix[i], bands_matrix[j], n_bands))
				continue;

			float similarity = signature_similarity(signature_matrix[i], signature_matrix[j], n_hashes);
			printf("[Docs %d - %d] Similarity MinHash: %.2f%%\n", i + offset, j + offset, 100.f * similarity);

		}

	return 0;
}

uint32_t min_hash_shingle(const char *filename, const int shingle_size, const int seed) {

	// Open file
	FILE *file = fopen(filename, "r");

	char *prev_words[shingle_size];
	char *shingle;
	uint32_t min_hash = UINT32_MAX, current_hash;

	// Set to null all previous words
	for (int i = 0; i < shingle_size; i++)
		prev_words[i] = NULL;

	// While shingles are read from file
	while ((shingle = read_shingle_from_file(file, shingle_size, prev_words))) {

		// Compute hash and save if min
		current_hash = murmur_hash(shingle, strlen(shingle), seed);
		if (current_hash < min_hash)
			min_hash = current_hash;

		// Free shingle
		free(shingle);
	}

	// Free previous words
	for (int i = 0; i < shingle_size; i++)
		free(prev_words[i]);

	// Close file
	fclose(file);

	return min_hash;
}