//
// Created by escac on 04/05/2024.
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "minhash.h"
#include "io_interface.h"
#include "utils.h"

void mh_main(struct Arguments args) {

	// Signature matrix - columns are documents, rows are hashes
	// Band matrix - columns are documents, rows are bands (hashed)
	uint32_t *signature_matrix;
	uint32_t *bands_matrix;

	if (args.verbose)
		printf("Allocating memory...\n");

	mh_allocate(args, &signature_matrix, &bands_matrix);

	if (args.verbose)
		printf("Opening report file...\n");

	// Open and write header to CSV file
	FILE *csv_file = fopen("results.csv", "w");
	fprintf(csv_file, "doc1,doc2,similarity\n");

	if (args.verbose)
		printf("Computing signatures...\n");

	// Compute the signatures of all documents
	mh_compute_signatures(args, signature_matrix);

	if (args.verbose)
		printf("Computing bands...\n");

	// Reduce the signatures to bands to faster comparison
	mh_compute_bands(args, signature_matrix, bands_matrix);

	if (args.verbose)
		printf("Comparing documents...\n");

	// Compare all document pairs and write to CSV file
	mh_compare(args, signature_matrix, bands_matrix, csv_file);

	if (args.verbose)
		printf("Done.\n");

	// Free memory and close files
	free(signature_matrix);
	free(bands_matrix);
	fclose(csv_file);

}

void mh_allocate(struct Arguments args, uint32_t **signature_matrix, uint32_t **bands_matrix) {

	const int n_bands = (int) (args.signature_size / args.n_band_rows);

	// Allocate matrices
	*signature_matrix = calloc(args.n_docs * args.signature_size, sizeof(uint32_t));
	*bands_matrix = calloc(args.n_docs * n_bands, sizeof(uint32_t));

	// Void matrices
	memset(*signature_matrix, 0, args.n_docs * args.signature_size * sizeof(uint32_t));
	memset(*bands_matrix, 0, args.n_docs * n_bands * sizeof(uint32_t));

}

void mh_compute_signatures(struct Arguments args, uint32_t *signature_matrix) {

	// Loop over all documents
	for (int i = 0; i < args.n_docs; ++i) {

		if (args.verbose && (i % args.verbose == 0 || i == args.n_docs - 1))
			printf("Computing signature for doc %d\n", i + args.doc_offset);

		// Compute the path of the document file (they are numbered)
		char *doc_filepath = (char *) malloc((strlen(args.directory) + 10) * sizeof(char));
		sprintf(doc_filepath, "%s/%d.txt", args.directory, i + args.doc_offset);

		// Write the signature of the i-th document in the i-th matrix row
		mh_document_signature(
				doc_filepath,
				(int) args.shingle_size,
				signature_matrix + i * args.signature_size,
				(int) args.signature_size,
				args.seed
		);

		// Free file path memory
		free(doc_filepath);
	}

}

void mh_document_signature(
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

void mh_compute_bands(struct Arguments args, const uint32_t *signature_matrix, uint32_t *bands_matrix) {

	const int n_bands = (int) (args.signature_size / args.n_band_rows);

	// Loop over all documents
	for (int i = 0; i < args.n_docs; ++i) {

		// Compute the bands of the i-th document
		for (int j = 0; j < n_bands; ++j) {

			// Compute the hash of the band
			uint32_t band_hash = 0;
			for (int k = 0; k < args.n_band_rows; ++k) {
				band_hash ^= signature_matrix[i * args.signature_size + j * args.n_band_rows + k];
			}

			// Save the band hash in the bands matrix
			bands_matrix[i * n_bands + j] = band_hash;
		}

	}

}

void mh_compare(struct Arguments args, uint32_t *signature_matrix, uint32_t *bands_matrix, FILE *csv) {

	const int n_bands = (int) (args.signature_size / args.n_band_rows);

	// Loop over all document pairs
	for (int i = 0; i < args.n_docs - 1; ++i)
		for (int j = i + 1; j < args.n_docs; ++j) {

			// Pointers to the bands of the two documents
			uint32_t *p_bands1 = bands_matrix + i * n_bands;
			uint32_t *p_bands2 = bands_matrix + j * n_bands;

			// Skip if not candidate pair
			if (!is_candidate_pair(p_bands1, p_bands2, n_bands))
				continue;

			// Pointers to the signatures of the two documents
			uint32_t *p_signature1 = signature_matrix + i * args.signature_size;
			uint32_t *p_signature2 = signature_matrix + j * args.signature_size;

			// Compute MinHash similarity and print if above threshold
			float similarity = signature_similarity(p_signature1, p_signature2, args.signature_size);
			if (similarity >= args.threshold)
				fprintf(csv, "%d,%d,%.4f\n", i + args.doc_offset, j + args.doc_offset, similarity);

		}

}
