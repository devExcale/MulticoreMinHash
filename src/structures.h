#ifndef MULTICOREMINHASH_STRUCTURES_H
#define MULTICOREMINHASH_STRUCTURES_H

struct Arguments {
	// Directory where to pull the documents from
	char *directory;
	// How many words in a shingle
	unsigned int shingle_size;
	// Number of hashes to compute for a document
	unsigned int signature_size;
	// Number of documents to process
	unsigned int n_docs;
	// Number of rows in each band
	unsigned int n_band_rows;
	// Hash function seed
	int seed;
	// After how many steps to print verbose information
	unsigned int verbose;
	// Minimum similarity threshold after which to print the score
	float threshold;
};

#endif //MULTICOREMINHASH_STRUCTURES_H
