#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "txt_reader.h"
#include "main.h"
#include "utils.h"

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
	printf("- Shingle size: %u\n", args.shingle_size);
	printf("- Signature size: %u\n", args.signature_size);
	printf("- Number of documents: %u\n", args.n_docs);
	printf("- Number of rows in bands: %u\n", args.n_band_rows);
	printf("- Seed: %d\n", args.seed);
	printf("- Verbose step: %u\n", args.verbose);
	printf("- Threshold: %.2f\n", args.threshold);
	printf("-----------------\n");
}

int main_min_hash(struct Arguments args) {

	const int n_hashes = args.signature_size;
	const int shingle_size = args.shingle_size;
	const int n_docs = args.n_docs;
	const int n_band_rows = args.n_band_rows;
	const int n_bands = n_hashes / n_band_rows;

	// Signature matrix - columns are documents, rows are hashes
	uint32_t *signature_matrix;
	size_t size_signature_matrix = n_docs * n_hashes * sizeof(uint32_t);

	// Band matrix - columns are documents, rows are bands (hashed)
	uint32_t *bands_matrix;
	size_t size_band_matrix = n_docs * n_bands * sizeof(uint32_t);

	// Print memory used by matrices (ceil approx.)
	if (args.verbose) {
		size_t mem_usage_kb = (size_signature_matrix + size_band_matrix) / 1000ULL + 1;
		printf("Memory used by processed documents: %zu KB\n", mem_usage_kb);
	}

	// Allocate and void matrices
	signature_matrix = malloc(size_signature_matrix);
	bands_matrix = malloc(size_band_matrix);
	memset(signature_matrix, 0, size_signature_matrix);
	memset(bands_matrix, 0, size_band_matrix);

	const int offset = 1;

	// [Compute signature matrix]
	// Loop over all documents
	for (int i = 0; i < n_docs; ++i) {

		if (args.verbose && (i % args.verbose == 0 || i == n_docs - 1))
			printf("Computing signature for doc %d\n", i + offset);

		// Compute the path of the document file (they are numbered)
		char *doc_filepath = (char *) malloc((strlen(args.directory) + 10) * sizeof(char));
		sprintf(doc_filepath, "%s\\%d.txt", args.directory, i + offset);

		// Write the signature of the i-th document in the i-th matrix row
		compute_document_signature(doc_filepath, shingle_size, signature_matrix + i * n_hashes, n_hashes, args.seed);

		// Free file path memory
		free(doc_filepath);
	}

	// [Compute bands matrix]
	// Loop over all documents
	for (int i = 0; i < n_docs; ++i) {

		if (args.verbose && (i % args.verbose == 0 || i == n_docs - 1))
			printf("Computing bands for doc %d\n", i + offset);

		// Compute hash for each band (simple XOR between hashes in the band)
		for (int j = 0; j < n_bands; ++j) {

			uint32_t band_hash = 0;

			for (int k = 0; k < n_band_rows; ++k)
				band_hash ^= *(signature_matrix + i * n_hashes + j * n_band_rows + k);

			// Write the band hash in the matrix
			*(bands_matrix + i * n_bands + j) = band_hash;
		}
	}

	printf("Starting comparison...\n");

	// Compare all pairs of documents
	for (int i = 0; i < n_docs - 1; ++i)
		for (int j = i + 1; j < n_docs; ++j) {

			// Pointers to the bands of the two documents
			uint32_t *p_bands1 = bands_matrix + i * n_bands;
			uint32_t *p_bands2 = bands_matrix + j * n_bands;

			if (!is_candidate_pair(p_bands1, p_bands2, n_bands))
				continue;

			// Pointers to the signatures of the two documents
			uint32_t *p_signature1 = signature_matrix + i * n_hashes;
			uint32_t *p_signature2 = signature_matrix + j * n_hashes;

			// Compute MinHash similarity and print if above threshold
			float similarity = signature_similarity(p_signature1, p_signature2, n_hashes);
			if (similarity >= args.threshold)
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

void compute_document_signature(
		const char *filepath,
		const int shingle_size,
		uint32_t *signature,
		const int signature_size,
		const int seed
) {

	// Open file
	FILE *file = fopen(filepath, "r");

	// Check if file was opened
	if (file == NULL) {
		printf("Error opening file %s\n", filepath);
		exit(2);
	}

	char *prev_words[shingle_size];
	char *shingle;
	uint32_t current_hash;

	// Set all signature values to max
	for (int i = 0; i < signature_size; i++) {
		signature[i] = UINT32_MAX;
	}

	// Set to null all previous words
	for (int i = 0; i < shingle_size; i++)
		prev_words[i] = NULL;

	// Read all shingles from file
	while ((shingle = read_shingle_from_file(file, shingle_size, prev_words))) {

		int shingle_len = strlen(shingle);

		// Compute document signature
		for (int i = 0; i < signature_size; ++i) {

			// Hash shingle and save if min
			current_hash = murmur_hash(shingle, shingle_len, seed * i);
			if (current_hash < signature[i])
				signature[i] = current_hash;
		}

		// Free shingle memory
		free(shingle);
	}

	// Free previous words memory
	for (int i = 0; i < shingle_size; i++)
		free(prev_words[i]);

	// Close file
	fclose(file);
}
