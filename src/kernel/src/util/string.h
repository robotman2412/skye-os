
#ifndef STRINGS_H
#define STRINGS_H
#include <stdint.h>
#include <stddef.h>


const void *memchr(const void *str, int c, size_t num);
int memcmp(const void *a, const void *b, size_t num);
void *memcpy(void *dest, const void* src, size_t num);
void *memmove(void *dest, const void* src, size_t num);
void *memset(void *dest, int c, size_t num);
char *strcat(char *dest, const char *src);
char *strncat(char *dest, const char *src, size_t num);
const char *strchr(const char *str, int c);
int strcmp(const char *a, const char *b);
int strncmp(const char *a, const char *b, size_t num);
// TODO? int strcoll(const char *a, const char *b);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);
// Number of characters for which a never matches b.
size_t strcspn(const char *a, const char *b);
// Returns the error string corresponding to errnum.
// TODO? char *strerror(int errnum);
size_t strlen(const char *str);
// First character in a matching any in b.
const char *strpbrk(const char *a, const char *b);
const char *strrchr(const char *str, int c);
// Number of characters for which a always matches b.
size_t strspn(const char *a, const char *b);
const char *strstr(const char *haystack, const char *needle);
// Split str by any in delim, returning the first token it was split into.
// Be there multiple tokens, and str is null, it will continue to return the next token.
// TODO: char *strtok(char *str, const char *delim);
// WTF?
// TODO? size_t strxfrm(char *dest, const char *src, size_t n);

#endif //STRINGS_H
