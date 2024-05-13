#include <omp.h>

#include "main.h"
#include "io_interface.h"
#include "minhash.h"

int main(int argc, char *argv[]) {

	// Read arguments and share among all processes
	struct Arguments args = input_arguments(argc, (const char **) argv);

	// Set desired number of threads
	omp_set_dynamic(0);
	omp_set_num_threads(args.proc.comm_sz);

	int num_threads;

	#pragma omp parallel default(none) shared(num_threads)
	num_threads = omp_get_num_threads();

	// Check if the number of threads was set correctly
	if (num_threads != args.proc.comm_sz) {
		printf("Error setting number of threads: %d/%d\n", num_threads, args.proc.comm_sz);
		return 7;
	}

	// Start the MinHash algorithm
	mh_main(args);

	return 0;
}
