#include <stdlib.h>
#include <string.h>
#include <mpi/mpi.h>

#include "main.h"
#include "io_interface.h"
#include "minhash.h"

int main(int argc, char *argv[]) {

	// MPI variables
	int my_rank, comm_sz;

	// Initialize MPI
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

	// Read arguments and share among all processes
	struct Arguments args = input_arguments_mpi(argc, (const char **) argv, my_rank, comm_sz);

	// Start the MinHash algorithm
	mh_main(args);

	// Close MPI
	MPI_Finalize();

	return 0;
}

struct Arguments input_arguments_mpi(const int argc, const char *argv[], const int my_rank, const int comm_sz) {

	struct Arguments args;
	int directory_len;

	// Main process reads the arguments
	if (my_rank == 0) {
		args = input_arguments(argc, argv);
		directory_len = (int) strlen(args.directory) + 1; // Including NULL terminator
	}

	// Broadcast the arguments (main sends, others read)
	MPI_Bcast(&args, sizeof(args), MPI_BYTE, 0, MPI_COMM_WORLD);

	// Broadcast directory string length (main sends, others read)
	MPI_Bcast(&directory_len, 1, MPI_INT, 0, MPI_COMM_WORLD);

	// Allocate memory for directory string
	if (my_rank != 0)
		args.directory = (char *) malloc((directory_len) * sizeof(char));

	// Broadcast directory path (main sends, others read)
	MPI_Bcast(args.directory, (int) directory_len, MPI_CHAR, 0, MPI_COMM_WORLD);

	// Assign process variables
	args.proc.my_rank = my_rank;
	args.proc.comm_sz = comm_sz;

	// my_n_docs: ceil(n_docs / n_procs) to other process and remainder to last process
	args.proc.doc_disp = args.n_docs / comm_sz + (args.n_docs % comm_sz != 0);
	args.proc.my_n_docs = (my_rank != comm_sz - 1) ? args.proc.doc_disp : args.n_docs - my_rank * args.proc.doc_disp;

	if (args.verbose && my_rank == 0)
		print_arguments(args);

	if (args.verbose) {
		int doc_start = my_rank * args.proc.doc_disp;
		int doc_end = doc_start + args.proc.my_n_docs - 1;
		printf("[Rank %2d] Range: %d - %d (%d)\n", my_rank, doc_start, doc_end, args.proc.my_n_docs);
	}

	return args;
}
