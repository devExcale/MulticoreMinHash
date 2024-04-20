#ifndef MULTICOREMINHASH_MAIN_H
#define MULTICOREMINHASH_MAIN_H

#include <stdio.h>
#include <stdint.h>
#include "txt_reader.h"

int main(const int argc, const char *argv[]);

// TODO: documentation
struct Arguments input_arguments(const int argc, const char *argv[]);

// TODO: documentation
struct Arguments default_arguments();

// TODO: documentation
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
 * @return exit code
 */
int main_min_hash(struct Arguments args);

#endif //MULTICOREMINHASH_MAIN_H
