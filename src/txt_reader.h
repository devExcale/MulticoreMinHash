#ifndef MULTICOREMINHASH_TEXT_READER_H
#define MULTICOREMINHASH_TEXT_READER_H

#include <stdio.h>

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

#endif //MULTICOREMINHASH_TEXT_READER_H
