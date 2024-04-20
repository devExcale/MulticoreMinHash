cmake_minimum_required(VERSION 3.27)

project(MulticoreMinHash
        DESCRIPTION "Implementation of the MinHash algorithm using parallel computing."
        VERSION 1.0.0
        LANGUAGES C)

set(CMAKE_C_STANDARD 11)

# Define the executable
add_executable(MulticoreMinHash
        src/main.c
        src/txt_reader.c
        src/utils.c
)

# Find and link with MPI libraries
find_package(MPI REQUIRED)
target_link_libraries(MulticoreMinHash PUBLIC MPI::MPI_C)

