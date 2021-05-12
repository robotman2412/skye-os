
#include "logging.h"

static void msp_append(char **buf, char *append, size_t maxlen) {
	size_t len = strlen(append);
	if (maxlen < len) len = maxlen;
	*buf = krealloc(*buf, strlen(*buf) + len + 1);
	strncat(*buf, append, len);
}

// Not the same as sprintf.
// Allocates memory for the output string.
// Formats: c (insert char), s (insert string), p (pointer)
// Number formats: d (decimal), x (hexadecimal), u (unsigned decimal)
// Number lengths: hh (char), h (short), l (long), z (size_t), default (int)
char *msprintf(char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	char *vec = va_msprintf(fmt, args);
	va_end(args);
	return vec;
}

// Not the same as sprintf.
// Allocates memory for the output string.
// Formats: c (insert char), s (insert string), p (pointer)
// Number formats: d (decimal), x (hexadecimal), u (unsigned decimal)
// Number lengths: hh (char), h (short), l (long), z (size_t), default (int)
char *va_msprintf(char *fmt, va_list args) {
	if (!fmt) return NULL;
	char *outbuf = kalloc(strlen(fmt) + 32);
	*outbuf = 0;
	char yesbuf[22];
	char *offs = fmt;
	while (*offs) {
		char *next = strchr(offs, '%');
		if (!next) {
			msp_append(&outbuf, offs, strlen(offs));
			break;
		} else {
			msp_append(&outbuf, offs, next - offs);
		}
		
		next ++;
		if (*next == '%') msp_append(&outbuf, "%", 1);
		else {
			int64_t value;
			char len = 4;
			switch (*next) {
				case ('h'):
					if (next[1] == 'h') {
						len = 1;
						next ++;
					} else {
						len = 2;
					}
					next ++;
					break;
				case ('l'):
					len = 8;
					next ++;
					break;
				case ('z'):
					len = 8;
					next ++;
					break;
			}
			if (*next == 'p' || *next == 's') len = 8;
			switch (len) {
				case (1):
					value = (int64_t) va_arg(args, int);
					break;
				case (2):
					value = (int64_t) va_arg(args, int);
					break;
				case (4):
					value = (int64_t) va_arg(args, int);
					break;
				case (8):
					value = (int64_t) va_arg(args, long);
					break;
			}
			switch (*next) {
				case ('c'):
					yesbuf[0] = (char) value;
					msp_append(&outbuf, yesbuf, 1);
					break;
				case ('s'):
					msp_append(&outbuf, (char *) value, strlen((char *) value));
					break;
				case ('p'):
					ltox(yesbuf, value, len * 2);
					msp_append(&outbuf, yesbuf, strlen(yesbuf));
					break;
				case ('x'):
					ltox(yesbuf, value, len * 2);
					msp_append(&outbuf, yesbuf, strlen(yesbuf));
					break;
				case ('u'):
					ultoa(yesbuf, value);
					msp_append(&outbuf, yesbuf, strlen(yesbuf));
					break;
				case ('d'):
					ltoa(yesbuf, value);
					msp_append(&outbuf, yesbuf, strlen(yesbuf));
					break;
			}
			next ++;
		}
		offs = next;
	}
	return outbuf;
}

static void logTimer(char *out) {
	//static uint64_t nanos = 0;
	//nanos += 23000;
	uint64_t nanos = hpet_getNanos();
	uint64_t micros = nanos / 1000;
	uint64_t millis = micros / 1000;
	micros %= 1000;
	char buf[22];
	ultoa(buf, millis);
	padn(buf, ' ', 4);
	memcpy(out, buf, 4);
	out[4] = '.';
	ultoa(buf, micros);
	padn(buf, '0', 4);
	memcpy(&out[5], buf, 4);
}

void logk(char *message) {
	char buf[] = "[   0.0000] [L] ";
	logTimer(&buf[1]);
	ttyFgCol = COLOR_GREEN;
	fbPrint(buf);
	ttyFgCol = COLOR_WHITE;
	fbPrint(message);
}

void warnk(char *message) {
	ttyFgCol = COLOR_GREEN;
	fbPrint("[   0.0000] ");
	ttyFgCol = COLOR_RED;
	fbPrint("[W] ");
	ttyFgCol = COLOR_WHITE;
	fbPrint(message);
}

void logkf(char *format, ...) {
	va_list args;
	va_start(args, format);
	char *vec = va_msprintf(format, args);
	va_end(args);
	logk(vec);
	kfree(vec);
}

void warnkf(char *format, ...) {
	va_list args;
	va_start(args, format);
	char *vec = va_msprintf(format, args);
	va_end(args);
	warnk(vec);
	kfree(vec);
}


