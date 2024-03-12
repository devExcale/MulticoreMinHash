#ifndef MULTICOREMINHASH_TEXT_READER_H
#define MULTICOREMINHASH_TEXT_READER_H

#include <stdio.h>

/**
 * Reads a word from an already opened file. <br>
 * Note: the returned string must be freed by the caller.
 * 
 * @param file The opened file
 * @return The read word
 */
char *read_word_from_file(FILE *file);


/**
 * Returns a string containing a shingle read from a given file. <br>
 * Note: The returned string's memory must be freed by the caller. <br>
 * Note: Once no more shingles are to be read, the caller must free the memory used by prev_words. <br>
 * Note: discarded (oldest) words from prev_words are freed by the function.
 * 
 * @param file Open file to read from
 * @param shingle_size Size of a shingle
 * @param prev_words An array of words (strings) previously read, handled by the function
 * @return 
 */
char *read_shingle_from_file(const FILE *file, const unsigned int shingle_size, char **prev_words);

#endif //MULTICOREMINHASH_TEXT_READER_H
