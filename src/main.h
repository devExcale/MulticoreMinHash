#ifndef MULTICOREMINHASH_MAIN_H
#define MULTICOREMINHASH_MAIN_H

#include <stdio.h>
#include <stdint.h>
#include "txt_reader.h"

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
	// After how many steps to print verbose information
	unsigned int verbose;
	// Minimum similarity threshold after which to print the score
	float threshold;
};

int main(const int argc, const char *argv[]);

/**
 * Reads the arguments passed to the program and returns them in a dedicated struct.
 * If an argument is not provided, the default value is used.
 * If the passed arguments are invalid, the program exits with a help message.
 *
 * @see default_arguments
 *
 * @param argc The number of arguments
 * @param argv The arguments
 * @return A struct containing the parsed arguments
 */
struct Arguments input_arguments(const int argc, const char *argv[]);

/**
 * Returns the default arguments used by the program.
 *
 * @return A struct containing the default arguments
 */
struct Arguments default_arguments();

/**
 * Print the given arguments.
 *
 * @param args The arguments to be printed
 */
void print_arguments(struct Arguments args);

/**
 * Read all shingles from a file, compute their hashes and return the hash with minimum value.
 * 
 * @param filename Absolute path of the file to read
 * @param shingle_size Size of a single
 * @param seed Initialization seed for the hash function
 * @return Minimum hash value
 */
uint32_t min_hash_shingle(const char *filename, const int shingle_size, const int seed);

/**
 * Performs the min_hash algorithm on multiple files with banding.
 *
 * @param args Arguments for the program
 * @return exit code
 */
int main_min_hash(struct Arguments args);

/**
 * Compute the signature of a document. <br>
 * The file is read word by word, and a shingle is built from the last n words read.
 * The shingle is then hashed and stored in the signature array.
 *
 * @param filepath Path to the document
 * @param shingle_size Size of a shingle
 * @param signature Array to store the signature
 * @param signature_size Size of the signature array
 * @param seed Seed for the hash function
 */
void compute_document_signature(
		const char *filepath,
		const int shingle_size,
		uint32_t *signature,
		const int signature_size,
		const int seed
);

#endif //MULTICOREMINHASH_MAIN_H