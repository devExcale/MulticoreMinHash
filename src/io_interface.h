#ifndef MULTICOREMINHASH_IO_INTERFACE_H
#define MULTICOREMINHASH_IO_INTERFACE_H

#include <stdio.h>

#include "structures.h"

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
 * Reads a word from an already opened p_file. <br>
 * Note: the returned string must be freed by the caller.
 * 
 * @param p_file The opened p_file
 * @return The read word, or NULL if no word could be read
 */
char *read_word_from_file(FILE *p_file);


/**
 * Returns a string containing a shingle read from a given file. <br>
 * Note: The returned string's memory must be freed by the caller. <br>
 * Note: Once no more shingles are to be read, the caller must free the memory used by p_words. <br>
 * Note: discarded (oldest) words from p_words are freed by the function.
 * 
 * @param file Open file to read from
 * @param shingle_size Size (in words) of a shingle
 * @param p_words An array of words (strings) previously read, handled by the function
 * @return The shingle read from the file, or NULL if no shingle could be read
 */
char *read_shingle_from_file(FILE *file, const unsigned int shingle_size, char **p_words);

#endif //MULTICOREMINHASH_IO_INTERFACE_H
