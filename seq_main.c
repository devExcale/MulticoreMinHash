#include <stdio.h>
#include "txt_reader.c"
#include "seq_main.h"

#define SHINGLE_SIZE 3
#define FILENAME1 "C:\\Users\\escac\\Projects\\Academics\\MulticoreMinHash\\docs\\Frank Soccer.txt"
#define FILENAME2 "C:\\Users\\escac\\Projects\\Academics\\MulticoreMinHash\\docs\\Liam Soccer.txt"

int main(const int argc, const char *argv[]) {

	printf("Shingle size: %d\n", SHINGLE_SIZE);

//    return print_shingles(FILENAME1, SHINGLE_SIZE);

	main_array_similarity(FILENAME1, FILENAME2, SHINGLE_SIZE);

	main_min_hash();

	return 0;
}

int main_min_hash() {

	const char *filename1 = FILENAME1;
	const char *filename2 = FILENAME2;

	const int N_HASHES = 300;

	int shingle_size = SHINGLE_SIZE;
	uint32_t hashes1[N_HASHES], hashes2[N_HASHES];

	for (int i = 0; i < N_HASHES; ++i) {
		hashes1[i] = min_hash_shingle(filename1, shingle_size, i * 17);
		hashes2[i] = min_hash_shingle(filename2, shingle_size, i * 17);
	}

	float similarity = array_similarity(hashes1, N_HASHES, hashes2, N_HASHES);
	printf("Similarity MinHash: %.2f%%\n", 100.f * similarity);

	return 0;
}

int print_shingles(const char *filename, const int shingle_size) {

	// Open file
	FILE *file = fopen(filename, "r");

	char *prev_words[shingle_size];
	char *shingle;

	// Set to null all previous words
	for (int i = 0; i < shingle_size; i++)
		prev_words[i] = NULL;

	// While shingles are read from file
	while ((shingle = read_shingle_from_file(file, shingle_size, prev_words))) {

		// Print shingle
		printf("%s\n", shingle);

		// Free shingle
		free(shingle);
	}

	return 0;
}

int main_array_similarity(const char *filename1, const char *filename2, const int shingle_size) {

	int hashes_size1 = 0, hashes_size2 = 0;
	uint32_t *hashes_arr1, *hashes_arr2;

	all_shingles_to_hashes_from_txt(filename1, shingle_size, &hashes_arr1, &hashes_size1);
	all_shingles_to_hashes_from_txt(filename2, shingle_size, &hashes_arr2, &hashes_size2);

	printf("Similarity TrueHash: %.2f%%\n",
		   100.f * array_similarity(hashes_arr1, hashes_size1, hashes_arr2, hashes_size2));

	// Free memory
	free(hashes_arr1);
	free(hashes_arr2);

	return 0;
}

void all_shingles_to_hashes_from_txt(const char *filename, const int shingle_size,
									 uint32_t **pp_hashes_arr, int *p_hashes_size) {

	// Open file
	FILE *file = fopen(filename, "r");

	char *prev_words[shingle_size];
	char *shingle;

	// Set initial number of shingles to 0 and create an array of 200 elements
	*p_hashes_size = 0;
	*pp_hashes_arr = malloc(1000 * sizeof(uint32_t));

	// Set to null all previous words
	for (int i = 0; i < shingle_size; i++)
		prev_words[i] = NULL;

	// While shingles are read from file
	while ((shingle = read_shingle_from_file(file, shingle_size, prev_words))) {

		// Compute and save hash
		*((*pp_hashes_arr) + (*p_hashes_size)++) = murmur_hash(shingle, strlen(shingle), 0);

		// Free shingle
		free(shingle);
	}

	// Reallocate array to the exact number of hashes
	*pp_hashes_arr = realloc(*pp_hashes_arr, *p_hashes_size * sizeof(uint32_t));

	// Free previous words
	for (int i = 0; i < shingle_size; i++)
		free(prev_words[i]);

	// Close file
	fclose(file);

	return;
}

uint32_t min_hash_shingle(const char *filename, const int shingle_size, const int seed) {

	// Open file
	FILE *file = fopen(filename, "r");

	char *prev_words[shingle_size];
	char *shingle;
	uint32_t min_hash = INT32_MAX, current_hash;

	// Set to null all previous words
	for (int i = 0; i < shingle_size; i++)
		prev_words[i] = NULL;

	// While shingles are read from file
	while ((shingle = read_shingle_from_file(file, shingle_size, prev_words))) {

		// Compute hash and save if min
		current_hash = murmur_hash(shingle, strlen(shingle), seed);
		if (current_hash < min_hash)
			min_hash = current_hash;

		// Free shingle
		free(shingle);
	}

	// Free previous words
	for (int i = 0; i < shingle_size; i++)
		free(prev_words[i]);

	// Close file
	fclose(file);

	return min_hash;
}

float array_similarity(const uint32_t *hashes_arr1, const int hashes_size1,
					   const uint32_t *hashes_arr2, const int hashes_size2) {
	int common = 0;

	for (int i = 0; i < hashes_size1; i++)
		for (int j = 0; j < hashes_size2; j++)
			if (hashes_arr1[i] == hashes_arr2[j]) {
				common++;
				break;
			}

	return (float) common / (float) (hashes_size1 + hashes_size2 - common);
}