#ifndef MULTICOREMINHASH_STRUCTURES_H
#define MULTICOREMINHASH_STRUCTURES_H

struct MultiProc {
	// ID of the current process
	int my_rank;
	// Number of processes
	int comm_sz;
	// Number of documents assigned to the current process
	int my_n_docs;
};

struct Arguments {
	// Directory where to pull the documents from
	char *directory;
	// Offset of the document index to start from (default starts from 0)
	int doc_offset;
	// How many words in a shingle
	int shingle_size;
	// Number of hashes to compute for a document
	int signature_size;
	// Number of documents to process
	int n_docs;
	// Number of rows in each band
	int n_band_rows;
	// Number of bands
	int n_bands;
	// Hash function seed
	int seed;
	// After how many steps to print verbose information (0 = disabled)
	unsigned int verbose;
	// Minimum similarity threshold after which to print the score
	float threshold;
	// MultiProc information
	struct MultiProc proc;
};

#endif //MULTICOREMINHASH_STRUCTURES_H
