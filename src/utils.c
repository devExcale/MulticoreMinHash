#include <ctype.h>
#include "utils.h"

uint32_t murmur_hash(const void* key, int len, uint32_t seed) {
	const uint32_t m = 0x5bd1e995;
	const int r = 24;

	uint32_t h = seed ^ len;

	const uint32_t* data = (const uint32_t *) key;
	const uint32_t* end = data + (len / 4);

	while(data != end) {
		uint32_t k = *data++;
		k *= m;
		k ^= k >> r;
		k *= m;

		h *= m;
		h ^= k;
	}

	const uint8_t* tail = (const uint8_t *) data;

	// ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
	switch(len & 3) {
		case 3:
			h ^= tail[2] << 16;
		case 2:
			h ^= tail[1] << 8;
		case 1:
			h ^= tail[0];
			h *= m;
	};

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}

void str_tolower_trim_nonalphanum(char* str) {
	// Used variables
	char* write_str = str;

	while(*str) {
		// Convert character to lowercase
		const char c = (char) tolower((int) *str);

		// Include alphanumeric characters only
		if(('a' <= c && c <= 'z') || ('0' <= c && c <= '9')) {
			*write_str = c;
			write_str++;
		}

		// Move to next character
		str++;
	}

	// Null-terminate the string
	*write_str = '\0';
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

float signature_similarity(const uint32_t *signature1, const uint32_t *signature2, const int signature_size) {
	int common = 0;

	for (int i = 0; i < signature_size; i++)
		if (signature1[i] == signature2[i])
			common++;

	return (float) common / (float) signature_size;
}

bool is_candidate_pair(const uint32_t *band1, const uint32_t *band2, const int n_bands) {

	for (int i = 0; i < n_bands; i++)
		if (band1[i] == band2[i])
			return true;

	return false;
}
