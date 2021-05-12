
#include "numbers.h"
#include "logging.h"

void padn(char *buf, char padding, size_t num) {
	size_t len = strlen(buf);
	if (len >= num) return;
	size_t numPad = num - len;
	memcpy(&buf[numPad], buf, len + 1);
	memset(buf, padding, numPad);
}

void ltox(char *out, long value, size_t num) {
	char hex[16] = "0123456789ABCDEF";
	for (size_t i = 0; i < num; i ++) {
		out[num - i - 1] = hex[value & 15];
		value >>= 4;
	}
	out[num] = 0;
}

void ultoa(char *out, unsigned long value) {
	int iterations = 64;
	int digits = 20;
	memset(out, 0, digits + 1);
	for (int iter = 0; iter < iterations; iter ++) {
		// Step 1: do the magic adding (redundant for first iteration)
		for (int x = digits - 1; x >= 0; x--) {
			out[x] += 3 * (out[x] >= 5);
		}
		// Step 2: shift all of it in.
		char carry = (value & 0x8000000000000000) != 0;
		for (int x = digits - 1; x >= 0; x--) {
			out[x] <<= 1;
			out[x] |= carry;
			carry = out[x] >= 0x10;
			out[x] &= 0x0f;
		}
		value <<= 1;
	}
	// Add 0x30 so as to convert to the decimals.
	for (int x = 0; x < digits; x++) {
		out[x] |= 0x30;
	}
	// Remove some leading zeroes.
	char *org = out;
	for (int x = 0; x < digits - 1; x++) {
		if (out[x] != '0') break;
		org ++;
	}
	strcpy(out, org);
}

void ltoa(char *out, long long value) {
	if (value < 0) {
		out[0] = '-';
		ultoa(&out[1], -value);
	} else {
		ultoa(out, value);
	}
}
