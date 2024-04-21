#ifndef MULTICOREMINHASH_UTILS_H
#define MULTICOREMINHASH_UTILS_H

#include <stdint.h>
#include <stdbool.h>

/**
 * MurmurHash3 implementation, credit to Austin Appleby.
 *
 * @param key Array containing data to hash
 * @param len Size of the array
 * @param seed Initialization seed
 * @return The hash value
 */
uint32_t murmur_hash(const void *key, int len, uint32_t seed);

/**
 * Converts a string to lowercase and removes non-alphanumeric characters.
 * The string is modified directly, without reallocating memory.
 * 
 * @param str String to convert
 */
void str_tolower_trim_nonalphanum(char *str);

/**
 * Computes the set (Jaccard) similarity of two arrays containing hash values.
 *
 * @param p_hashes1 Address of the first array
 * @param n_hashes1 Size of the first array
 * @param p_hashes2 Address of the second array
 * @param n_hashes2 Size of the second array
 * @return The similarity value
 */
float array_similarity(const uint32_t *p_hashes1, const int n_hashes1,
					   const uint32_t *p_hashes2, const int n_hashes2);

/**
 * Computes the similarity of two signatures.
 *
 * @param p_signature1 Address of the first signature array
 * @param p_signature2 Address of the second signature array
 * @param signature_size Size of the arrays
 *
 * @return The similarity value
 */
float signature_similarity(const uint32_t *p_signature1, const uint32_t *p_signature2, const int signature_size);

/**
 * Checks whether two documents are candidate pairs by comparing their bands.
 * If just a single band is equal, the documents are considered candidate pairs.
 *
 * @param p_bands1 Address of the first bands array
 * @param p_bands2 Address of the second bands array
 * @param n_bands Number of bands
 *
 * @return True if the bands are candidate pairs, false otherwise
 */
bool is_candidate_pair(const uint32_t *p_bands1, const uint32_t *p_bands2, const int n_bands);

#endif //MULTICOREMINHASH_UTILS_H
