#include "main.h"

#include "io_interface.h"
#include "minhash.h"

int main(const int argc, const char *argv[]) {

	// Get arguments
	struct Arguments args = input_arguments(argc, argv);

	if (args.verbose)
		print_arguments(args);

	mh_main(args);

	return 0;
}
