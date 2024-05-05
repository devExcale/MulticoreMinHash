#ifndef MULTICOREMINHASH_MAIN_H
#define MULTICOREMINHASH_MAIN_H

int main(int argc, char *argv[]);

/**
 * Let the main process read the program's arguments and broadcast them to the other processes.
 *
 * @param argc Program's argument count
 * @param argv Program's arguments
 * @param my_rank MPI rank
 * @param comm_sz MPI size
 * @return The arguments read by the main process
 */
struct Arguments input_arguments_mpi(const int argc, const char *argv[], const int my_rank, const int comm_sz);

#endif //MULTICOREMINHASH_MAIN_H
