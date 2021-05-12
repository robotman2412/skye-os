
#ifndef NUMBERS_H
#define NUMBERS_H

#include "framebuffer.h"
#include "string.h"
#include "heap.h"
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

void padn(char *buf, char padding, size_t num);

void ltox(char *out, long value, size_t num);

void ultoa(char *out, unsigned long value);

void ltoa(char *out, long long value);

#endif //NUMBERS_H
