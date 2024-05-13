#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <mpi/mpi.h>

#include "minhash.h"
#include "io_interface.h"
#include "utils.h"

void mh_main(struct Arguments args) {

	// Signature matrix - columns are documents, rows are hashes
	// Band matrix - columns are documents, rows are bands (hashed)
	uint32_t *signature_matrix;
	uint32_t *bands_matrix;

	uint8_t verbose = args.verbose && args.proc.my_rank == 0;

	if (verbose)
		printf("Allocating memory...\n");

	mh_allocate(args, &signature_matrix, &bands_matrix);

	if (verbose)
		printf("Opening report file...\n");

	// Open and write header to CSV file
	FILE *csv_file = fopen("results.csv", "w");
	fprintf(csv_file, "doc1,doc2,similarity\n");

	if (verbose)
		printf("Computing signatures...\n");

	// Compute the signatures of all documents
	mh_compute_signatures(args, signature_matrix);

	if (verbose)
		printf("Computing bands...\n");

	// Reduce the signatures to bands to faster comparison
	mh_compute_bands(args, signature_matrix, bands_matrix);

	if (verbose)
		printf("Synchonizing memory...\n");

	// Send other processes results to main process
	sync_mem_mpi(args, signature_matrix, bands_matrix);

	// End other processes
	if (args.proc.my_rank != 0) {
		free(signature_matrix);
		free(bands_matrix);
		return;
	}

	if (verbose)
		printf("Comparing documents...\n");

	// Compare all document pairs and write to CSV file
	mh_compare(args, signature_matrix, bands_matrix, csv_file);

	if (verbose)
		printf("Done.\n");

	// Free memory and close files
	free(signature_matrix);
	free(bands_matrix);
	fclose(csv_file);

}

void mh_allocate(struct Arguments args, uint32_t **pp_signature_matrix, uint32_t **pp_bands_matrix) {

	// Allocate matrices (calloc initializes to 0 all memory)
	if (args.proc.my_rank == 0) {
		// Allocate space for all documents
		*pp_signature_matrix = calloc(args.n_docs * args.signature_size, sizeof(uint32_t));
		*pp_bands_matrix = calloc(args.n_docs * args.n_bands, sizeof(uint32_t));
	} else {
		// Allocate space for the documents assigned to this process
		*pp_signature_matrix = calloc(args.proc.my_n_docs * args.signature_size, sizeof(uint32_t));
		*pp_bands_matrix = calloc(args.proc.my_n_docs * args.n_bands, sizeof(uint32_t));
	}

}

void mh_compute_signatures(struct Arguments args, uint32_t *p_signature_matrix) {

	const int general_offset = args.n_docs / args.proc.comm_sz + (args.n_docs % args.proc.comm_sz != 0);
	const int my_doc_offset = args.doc_offset + args.proc.my_rank * general_offset;

	size_t filepath_len = strlen(args.directory) + 20UL;
	char doc_filepath[filepath_len];

	// Loop over all documents assigned to the current process
	for (int i = 0; i < args.proc.my_n_docs; ++i) {

		if (args.verbose && (i % args.verbose == 0))
			printf("Computing signature for doc %d\n", i + my_doc_offset);

		// Compute the path of the document file (they are numbered)
		sprintf(doc_filepath, "%s/%d.txt", args.directory, i + my_doc_offset);

		// Write the signature of the i-th document in the i-th matrix row
		mh_document_signature(
				doc_filepath,
				(int) args.shingle_size,
				p_signature_matrix + i * args.signature_size,
				(int) args.signature_size,
				args.seed
		);
	}

}

void mh_document_signature(
		const char *filepath,
		const int shingle_size,
		uint32_t *signature,
		const int signature_size,
		const int seed
) {

	// Open file
	FILE *file = fopen(filepath, "r");

	// Check if file was opened
	if (file == NULL) {
		printf("Error opening file %s\n", filepath);
		exit(2);
	}

	char *prev_words[shingle_size];
	char *shingle;
	uint32_t current_hash;

	// Set all signature values to max
	for (int i = 0; i < signature_size; i++) {
		signature[i] = UINT32_MAX;
	}

	// Set to null all previous words
	for (int i = 0; i < shingle_size; i++)
		prev_words[i] = NULL;

	// Read all shingles from file
	while ((shingle = read_shingle_from_file(file, shingle_size, prev_words))) {

		int shingle_len = strlen(shingle);

		// Compute document signature
		for (int i = 0; i < signature_size; ++i) {

			// Hash shingle and save if min
			current_hash = murmur_hash(shingle, shingle_len, seed * i);
			if (current_hash < signature[i])
				signature[i] = current_hash;
		}

		// Free shingle memory
		free(shingle);
	}

	// Free previous words memory
	for (int i = 0; i < shingle_size; i++)
		free(prev_words[i]);

	// Close file
	fclose(file);
}

void mh_compute_bands(struct Arguments args, const uint32_t *p_signature_matrix, uint32_t *p_bands_matrix) {

	// Loop over all documents
	for (int i = 0; i < args.proc.my_n_docs; ++i) {

		// Compute the bands of the i-th document
		for (int j = 0; j < args.n_bands; ++j) {

			// Compute the hash of the band
			uint32_t band_hash = 0;
			for (int k = 0; k < args.n_band_rows; ++k) {
				band_hash ^= p_signature_matrix[i * args.signature_size + j * args.n_band_rows + k];
			}

			// Save the band hash in the bands matrix
			p_bands_matrix[i * args.n_bands + j] = band_hash;
		}

	}

}

void sync_mem_mpi(struct Arguments args, uint32_t *p_signature_matrix, uint32_t *p_bands_matrix) {

	const int n_docs = args.n_docs;
	const int size_sig = args.signature_size;
	const int comm_sz = args.proc.comm_sz;
	const int my_n_docs = args.proc.my_n_docs;

	// Synchronize computed signatures and bands
	if (args.proc.my_rank != 0) {

		if (args.verbose) {
			printf("[Rank %d] Sending %d signature uint32_t\n", args.proc.my_rank, my_n_docs * size_sig);
			printf("[Rank %d] Sending %d bands uint32_t\n", args.proc.my_rank, my_n_docs * args.n_bands);
		}

		// Send signature matrix
		MPI_Send((void *) p_signature_matrix, my_n_docs * size_sig, MPI_UNSIGNED, 0, 0, MPI_COMM_WORLD);
		// Send bands matrix
		MPI_Send((void *) p_bands_matrix, my_n_docs * args.n_bands, MPI_UNSIGNED, 0, 0, MPI_COMM_WORLD);

	} else {
		// Receive all documents from other processes
		for (int i = 1; i < args.proc.comm_sz; ++i) {

			// Number of documents to receive (my_n_docs of process i)
			int recv_n_docs = n_docs / comm_sz + (n_docs % comm_sz != 0);
			if (i == comm_sz - 1)
				recv_n_docs = n_docs - i * recv_n_docs;

			if (args.verbose) {
				printf("[Rank 0] From %d receiving %d signature uint32_t\n", i, recv_n_docs * size_sig);
				printf("[Rank 0] From %d receiving %d bands uint32_t\n", i, recv_n_docs * args.n_bands);
			}

			uint32_t *p_recv_signature_matrix = p_signature_matrix + i * my_n_docs * size_sig;
			uint32_t *p_recv_bands_matrix = p_bands_matrix + i * my_n_docs * args.n_bands;

			// Receive signature matrix
			MPI_Recv((void *) p_recv_signature_matrix, recv_n_docs * size_sig,
					 MPI_UNSIGNED, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			// Receive bands matrix
			MPI_Recv((void *) p_recv_bands_matrix, recv_n_docs * args.n_bands,
					 MPI_UNSIGNED, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}

	if (args.verbose)
		printf("[Rank %d] Memory synchronized.\n", args.proc.my_rank);

}

void mh_compare(struct Arguments args, uint32_t *p_signature_matrix, uint32_t *p_bands_matrix, FILE *f_csv) {

	const int n_bands = (int) (args.signature_size / args.n_band_rows);

	// Loop over all document pairs
	for (int i = 0; i < args.n_docs - 1; ++i)
		for (int j = i + 1; j < args.n_docs; ++j) {

			// Pointers to the bands of the two documents
			uint32_t *p_bands1 = p_bands_matrix + i * n_bands;
			uint32_t *p_bands2 = p_bands_matrix + j * n_bands;

			// Skip if not candidate pair
			if (!is_candidate_pair(p_bands1, p_bands2, n_bands))
				continue;

			// Pointers to the signatures of the two documents
			uint32_t *p_signature1 = p_signature_matrix + i * args.signature_size;
			uint32_t *p_signature2 = p_signature_matrix + j * args.signature_size;

			// Compute MinHash similarity and print if above threshold
			float similarity = signature_similarity(p_signature1, p_signature2, args.signature_size);
			if (similarity >= args.threshold)
				fprintf(f_csv, "%d,%d,%.4f\n", i + args.doc_offset, j + args.doc_offset, similarity);

		}

}
