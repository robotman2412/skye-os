
#ifndef LOGGING_H
#define LOGGING_H

#include "framebuffer.h"
#include "numbers.h"
#include "timer.h"
#include "heap.h"
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

// Not the same as sprintf.
// Allocates memory for the output string.
// Formats: c (insert char), s (insert string), p (pointer)
// Number formats: d (decimal), x (hexadecimal), u (unsigned decimal)
// Number lengths: hh (char), h (short), l (long), z (size_t), default (int)
char *va_msprintf(char *format, va_list args);

// Not the same as sprintf.
// Allocates memory for the output string.
// Formats: c (insert char), s (insert string), p (pointer)
// Number formats: d (decimal), x (hexadecimal), u (unsigned decimal)
// Number lengths: hh (char), h (short), l (long), z (size_t), default (int)
char *msprintf(char *format, ...);

void logk(char *message);
void logkf(char *format, ...);
void errk(char *message);
void errkf(char *format, ...);
void warnk(char *message);
void warnkf(char *format, ...);
void debugk(char *message);
void debugkf(char *format, ...);

#endif //LOGGING_H
