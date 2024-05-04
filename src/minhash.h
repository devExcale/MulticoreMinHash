//
// Created by escac on 04/05/2024.
//

#ifndef MULTICOREMINHASH_MINHASH_H
#define MULTICOREMINHASH_MINHASH_H

#include <stdint.h>

#include "structures.h"

// TODO: documentation
void mh_main(struct Arguments args);

// TODO: documentation
void mh_allocate(struct Arguments args, uint32_t **signature_matrix, uint32_t **bands_matrix);

// TODO: documentation
void mh_compute_signatures(struct Arguments args, uint32_t *signature_matrix);

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
void mh_document_signature(
		const char *filepath,
		const int shingle_size,
		uint32_t *signature,
		const int signature_size,
		const int seed
);

// TODO: documentation
void mh_compute_bands(struct Arguments args, const uint32_t *signature_matrix, uint32_t *bands_matrix);

// TODO: documentation
void mh_compare(struct Arguments args, uint32_t *signature_matrix, uint32_t *bands_matrix, FILE *csv);

#endif //MULTICOREMINHASH_MINHASH_H
