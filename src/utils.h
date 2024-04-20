#ifndef MULTICOREMINHASH_UTILS_H
#define MULTICOREMINHASH_UTILS_H

#include <stdint.h>

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

#endif //MULTICOREMINHASH_UTILS_H
