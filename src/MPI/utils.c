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

float array_similarity(const uint32_t *p_hashes1, const int n_hashes1,
					   const uint32_t *p_hashes2, const int n_hashes2) {
	int common = 0;

	for (int i = 0; i < n_hashes1; i++)
		for (int j = 0; j < n_hashes2; j++)
			if (p_hashes1[i] == p_hashes2[j]) {
				common++;
				break;
			}

	return (float) common / (float) (n_hashes1 + n_hashes2 - common);
}

float signature_similarity(const uint32_t *p_signature1, const uint32_t *p_signature2, const int signature_size) {
	int common = 0;

	for (int i = 0; i < signature_size; i++)
		if (p_signature1[i] == p_signature2[i])
			common++;

	return (float) common / (float) signature_size;
}

bool is_candidate_pair(const uint32_t *p_bands1, const uint32_t *p_bands2, const int n_bands) {

	for (int i = 0; i < n_bands; i++)
		if (p_bands1[i] == p_bands2[i])
			return true;

	return false;
}
