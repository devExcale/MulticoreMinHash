#ifndef MULTICOREMINHASH_MAIN_H
#define MULTICOREMINHASH_MAIN_H

#include <stdio.h>
#include <stdint.h>
#include "io_interface.h"

int main(const int argc, const char *argv[]);

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
