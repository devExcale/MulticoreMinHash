#ifndef MULTICOREMINHASH_UTILS_H
#define MULTICOREMINHASH_UTILS_H

#include <stdint.h>
#include <stdbool.h>

/**
 * MurmurHash3 implementation, TODO: check reference
 * @param key Array containing data to hash
 * @param len Size of the array
 * @param seed Initialization seed
 * @return 
 */
uint32_t murmur_hash(const void *key, int len, uint32_t seed);

/**
 * Converts a string to lowercase and removes non-alphanumeric characters.
 * 
 * @param str String to convert
 */
void str_tolower_trim_nonalphanum(char *str);

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
bool is_candidate_pair(const uint32_t *band1, const uint32_t *band2, const int n_bands);

#endif //MULTICOREMINHASH_UTILS_H
