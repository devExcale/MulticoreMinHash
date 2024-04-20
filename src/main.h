#ifndef MULTICOREMINHASH_MAIN_H
#define MULTICOREMINHASH_MAIN_H

#include <stdio.h>
#include <stdint.h>
#include "txt_reader.h"

int main(const int argc, const char *argv[]);

// TODO: documentation
struct Arguments default_arguments();

// TODO: documentation
void print_arguments(struct Arguments args);

/**
 * Reads all shingles from a txt file, and computes their hashes. <br>
 * Note: the array must be freed by the caller.
 * 
 * @param filename Complete path of the file to read
 * @param shingle_size Size of a shingle
 * @param pp_hashes_arr Pointer to an array variable that will hold the hashes
 * @param p_hashes_size Pointer to an int variable, the length of the hashes array
 */
void all_shingles_to_hashes_from_txt(const char *filename, const int shingle_size,
									 uint32_t **pp_hashes_arr, int *p_hashes_size);

/**
 * Computes the set similarity of two arrays.
 * 
 * @param hashes_arr1 Address of the first array
 * @param hashes_size1 Size of the first array
 * @param hashes_arr2 Address of the second array
 * @param hashes_size2 Size of the second array
 * @return The similarity value
 */
float array_similarity(const uint32_t *hashes_arr1, const int hashes_size1,
					   const uint32_t *hashes_arr2, const int hashes_size2);

/**
 * Computes the similarity of two signatures.
 *
 * @param signature1 Address of the first signature
 * @param signature2 Address of the second signature
 * @param signature_size Size of the signatures
 * @return The similarity value
 */
float signature_similarity(const uint32_t *signature1, const uint32_t *signature2, const int signature_size);

// TODO: documentation
bool is_candidate_pair(const uint32_t *signature_band1, const uint32_t *signature_band2, const int n_bands);

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
 * @return exit code
 */
int main_min_hash(struct Arguments args);

/**
 * Print all shingles of a file.
 * Used for debug.
 * 
 * @param filename Absolute path of the file to read
 * @param shingle_size Size of a shingle
 * @return exit code
 */
int print_shingles(const char *filename, const int shingle_size);

/**
 * Executes the array_similarity function on the hashes arrays of two files.
 * 
 * @param filename1 Absolute path of the first file
 * @param filename2 Absolute path of the second file
 * @param shingle_size Size of a shingle
 * 
 * @return exit code
 */
int main_array_similarity(const char *filename1, const char *filename2, const int shingle_size);

#endif //MULTICOREMINHASH_MAIN_H
