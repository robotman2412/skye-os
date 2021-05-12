
#include <stdint.h>
#include "string.h"

// Because i have absolutely no libraries, i need to re-implement this.

void *memchr(const void *str, int c, size_t num) {
	if (str == NULL) return NULL;
	while (num > 0) {
		if (*(uint8_t*)str == (uint8_t) c) {
			return str;
		}
		str ++;
		num --;
	}
	return NULL;
}

int memcmp(const void *a, const void *b, size_t num) {
	if (a == NULL || b == NULL) return 1;
	for (size_t i = 0; i < num; i++) {
		int res = ((uint8_t*)a)[i] - ((uint8_t*)b)[i];
		if (res < 0) return -1;
		else if (res > 0) return 1;
	}
	return 0;
}

void *memcpy(const void *dest, const void* src, size_t num) {
	if (dest == NULL || src == NULL) return NULL;
	// TODO: see if there is some hardware for this?
	if (src > dest) {
		for (size_t i = 0; i < num; i++) {
			((uint8_t*)dest)[i] = ((uint8_t*)src)[i];
		}
	} else {
		for (size_t i = num - 1; i > 0; i--) {
			((uint8_t*)dest)[i] = ((uint8_t*)src)[i];
		}
		((uint8_t*)dest)[0] = ((uint8_t*)src)[0];
	}
	return dest;
}

void *memmove(const void *dest, const void* src, size_t num) {
	return memcpy(dest, src, num);
}

void *memset(void *dest, int c, size_t num) {
	if (dest == NULL) return NULL;
	// TODO: see if there is some hardware for this?
	for (size_t i = 0; i < num; i++) {
		((uint8_t*)dest)[i] = (uint8_t) c;
	}
	return dest;
}

char *strcat(char *dest, const char *src) {
	if (dest == NULL || src == NULL) return NULL;
	size_t destLen = strlen(dest);
	size_t srcLen = strlen(src);
	memcpy(&dest[destLen], src, srcLen);
	dest[destLen+srcLen] = 0;
}

char *strncat(char *dest, const char *src, size_t num) {
	if (dest == NULL || src == NULL) return NULL;
	size_t destLen = strlen(dest);
	size_t srcLen = strlen(src);
	if (srcLen > num) srcLen = num;
	memcpy(&dest[destLen], src, srcLen);
	dest[destLen+srcLen] = 0;
}

char *strchr(const char *str, int c) {
	if (str == NULL) return NULL;
	while (*str) {
		if (*str == c) {
			return str;
		}
		str ++;
	}
	return NULL;
}

int strcmp(const char *a, const char *b) {
	if (a == NULL || b == NULL) return a != b;
	while (1) {
		int res = *a - *b;
		if (res < 0) return -1;
		else if (res > 0) return 1;
		if (*a == 0) return 0;
		a++;
		b++;
	}
}

int strncmp(const char *a, const char *b, size_t num) {
	if (a == NULL || b == NULL) return a != b;
	while (num --) {
		int res = *a - *b;
		if (res < 0) return -1;
		else if (res > 0) return 1;
		if (*a == 0) return 0;
		a++;
		b++;
	}
	return 0;
}

// TODO? int strcoll(const char *a, const char *b);

char *strcpy(char *dest, const char *src) {
	if (dest == NULL || src == NULL) return NULL;
	return memcpy(dest, src, strlen(src) + 1);
}

char *strncpy(char *dest, const char *src, size_t num) {
	if (dest == NULL || src == NULL) return NULL;
	size_t len = strlen(src);
	if (len > num) len = num;
	return memcpy(dest, src, len);
}

// Number of characters for which a never matches characters in b.
size_t strcspn(const char *a, const char *b) {
	if (a == NULL) return 0;
	else if (b == NULL) {
		if (a == NULL) {
			return 0;
		} else {
			return strlen(a);
		}
	}
	size_t bLen = strlen(b);
	size_t num = 0;
	while (*a) {
		for (size_t i = 0; i < bLen; i++) {
			if (*a == b[i]) return num;
		}
		num ++;
		a ++;
	}
	return num;
}

// Returns the error string corresponding to errnum.
// TODO? char *strerror(int errnum);

size_t strlen(const char *str) {
	if (str == NULL) return 0;
	size_t num = 0;
	while (*str) {
		num ++;
		str ++;
	}
	return num;
}

// First character in a matching any in b.
char *strpbrk(const char *a, const char *b) {
	if (a == NULL || b == NULL) return NULL;
	size_t bLen = strlen(b);
	while (*a) {
		for (size_t i = 0; i < bLen; i++) {
			if (*a == b[i]) return a;
		}
	}
	return NULL;
}

char *strrchr(const char *str, int c) {
	if (str == NULL) return NULL;
	char *lastOccurance = NULL;
	while (*str) {
		if (*str == c) lastOccurance = str;
		str ++;
	}
	return lastOccurance;
}

// Number of characters for which a always matches characters in b.
size_t strspn(const char *a, const char *b) {
	if (a == NULL) return 0;
	else if (b == NULL) {
		if (a == NULL) {
			return 0;
		} else {
			return strlen(a);
		}
	}
	size_t bLen = strlen(b);
	size_t num = 0;
	while (*a) {
		for (size_t i = 0; i < bLen; i++) {
			if (*a == b[i]) goto l0;
		}
		return num;
		l0:
		num ++;
		a ++;
	}
	return num;
}

char *strstr(const char *haystack, const char *needle) {
	if (haystack == NULL || needle == NULL) return NULL;
	size_t needleIndex = 0;
	while (*haystack) {
		if (needle[needleIndex] == 0) {
			return haystack;
		} else if (needle[needleIndex] == *haystack) {
			needleIndex ++;
		} else {
			needleIndex = 0;
		}
	}
	return NULL;
}

// Split str by any in delim, returning the first token it was split into.
// Be there multiple tokens, and str is null, it will continue to return the next token.
// TODO: char *strtok(char *str, const char *delim);

// WTF?
// TODO? size_t strxfrm(char *dest, const char *src, size_t n);
