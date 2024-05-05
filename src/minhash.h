#ifndef MULTICOREMINHASH_MINHASH_H
#define MULTICOREMINHASH_MINHASH_H

#include <stdint.h>

#include "structures.h"

/**
 * Perform the MinHash algorithm on the given arguments.
 * If verbose, progess will be printed on stdout.
 * The algorithm's results will be written to a CSV file.
 *
 * @param args Algorithm's arguments
 */
void mh_main(struct Arguments args);

/**
 * Allocate memory for the signature and bands matrices.
 * Memory must be freed by the caller after usage.
 *
 * @param args Algorithm's arguments
 * @param pp_signature_matrix Address to the signature matrix's pointer
 * @param pp_bands_matrix Address to the bands matrix's pointer
 */
void mh_allocate(struct Arguments args, uint32_t **pp_signature_matrix, uint32_t **pp_bands_matrix);

/**
 * Compute the signature matrix of all documents.
 *
 * @param args Algorithm's arguments
 * @param p_signature_matrix Pointer to the signature matrix
 */
void mh_compute_signatures(struct Arguments args, uint32_t *p_signature_matrix);

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

/**
 * Compute the bands matrix from the signature matrix.
 *
 * @param args Algorithm's arguments
 * @param p_signature_matrix Pointer to the signature matrix
 * @param p_bands_matrix Pointer to the bands matrix
 */
void mh_compute_bands(struct Arguments args, const uint32_t *p_signature_matrix, uint32_t *p_bands_matrix);

/**
 * Transfer the matrices from the other processes to the main process.
 *
 * @param args Algorithm's arguments
 * @param p_signature_matrix Pointer to the signature matrix
 * @param p_bands_matrix Pointer to the bands matrix
 */
void sync_mem_mpi(struct Arguments args, uint32_t *p_signature_matrix, uint32_t *p_bands_matrix);

/**
 * Compare all document pairs and write candidate pairs to a CSV file.
 * A candidate pair is a pair of documents whose at least one band is equal.
 * The similarity score of a pair is computed by comparing the signatures of the two documents.
 *
 * @param args Algorithm's arguments
 * @param p_signature_matrix Pointer to the signature matrix
 * @param p_bands_matrix Pointer to the bands matrix
 * @param f_csv Open CSV file where to write the results
 */
void mh_compare(struct Arguments args, uint32_t *p_signature_matrix, uint32_t *p_bands_matrix, FILE *f_csv);

#endif //MULTICOREMINHASH_MINHASH_H
