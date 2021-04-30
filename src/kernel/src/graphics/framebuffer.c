
#include "../kernel.h"
#include "framebuffer.h"
#include "font7x9.h"
#include <stdint.h>
#include <stddef.h>

uint32_t ansiColors[16] = {
	COLOR_BLACK,
	COLOR_DARK_RED,
	COLOR_DARK_GREEN,
	COLOR_DARK_YELLOW,
	COLOR_DARK_BLUE,
	COLOR_DARK_MAGENTA,
	COLOR_DARK_CYAN,
	COLOR_LIGHT_GRAY,
	COLOR_DARK_GRAY,
	COLOR_RED,
	COLOR_GREEN,
	COLOR_YELLOW,
	COLOR_BLUE,
	COLOR_MAGENTA,
	COLOR_CYAN,
	COLOR_WHITE
};

// Variables.
uint32_t* framebuf;
uint64_t framebufWidth;
uint64_t framebufHeight;
char doubleTextSize = 0;

// TTY variables.
uint32_t ttyFgCol = COLOR_WHITE;
uint32_t ttyBgCol = COLOR_BLACK;
int ttyMaxX = 0;
int ttyMaxY = 0;
int ttyXPos = 0;
int ttyYPos = 0;

static char ttyLastChar = 0;

void fbPrint(char *text) {
	while (*text) {
		fbPutc(*text);
		text ++;
	}
}

void fbPutc(char text) {
	if (text == '\n') {
		if (ttyLastChar != '\r') {
			fbNewln();
		}
	} else if (text == '\r') {
		fbNewln();
	} else if (doubleTextSize) {
		fbChar((ttyXPos ++) * FONT_WIDTH2, ttyYPos * FONT_HEIGHT2, text, ttyFgCol, ttyBgCol);
	} else {
		fbChar((ttyXPos ++) * FONT_WIDTH, ttyYPos * FONT_HEIGHT, text, ttyFgCol, ttyBgCol);
	}
	ttyLastChar = text;
}

void fbPuthex(uint64_t value, int num) {
	if (num < 1) num = 1;
	else if (num > 16) num = 16;
	char *hex = "0123456789ABCDEF";
	for (int i = 0; i < num; i++) {
		uint64_t digit = value >> (num - i - 1) * 4;
		fbPutc(hex[digit & 0x0f]);
	}
}

void fbNewln() {
	ttyXPos = 0;
	ttyYPos ++;
	if (ttyYPos > ttyMaxY) {
		int dest = 0;
		int src = framebufWidth * (doubleTextSize ? FONT_HEIGHT2 : FONT_HEIGHT);
		int len = framebufWidth * framebufHeight - src;
		for (int i = 0; i < len; i++) {
			framebuf[dest] = framebuf[src];
		}
	}
}

void fbText(int x, int y, char *text, uint32_t color) {
	fbTextb(x, y, text, color, 0xff000000);
}

void fbTextb(int x, int y, char *text, uint32_t color, uint32_t bgColor) {
	int inc = doubleTextSize ? FONT_WIDTH2 : FONT_WIDTH;
	while (*text) {
		fbChar(x, y, *text, color, bgColor);
		text ++;
		x += inc;
	}
}

void fbChar(int x, int y, char text, uint32_t color, uint32_t bgColor) {
	if (text & 0x80) text = 0;
	int offs = text * 9;
	if (doubleTextSize) {
		for (int _y = 0; _y < 9; _y ++) {
			for (int _x = 0; _x < 7; _x ++) {
				char val = glyphs[_y + offs] & (1 << _x);
				fbSet(x + 0 + (_x << 1), y + 0 + (_y << 1), val ? color : bgColor);
				fbSet(x + 1 + (_x << 1), y + 0 + (_y << 1), val ? color : bgColor);
				fbSet(x + 0 + (_x << 1), y + 1 + (_y << 1), val ? color : bgColor);
				fbSet(x + 1 + (_x << 1), y + 1 + (_y << 1), val ? color : bgColor);
			}
		}
	} else {
		for (int _y = 0; _y < 9; _y ++) {
			for (int _x = 0; _x < 7; _x ++) {
				char val = glyphs[_y + offs] & (1 << _x);
				fbSet(x + _x, y + _y, val ? color : bgColor);
			}
		}
	}
}

void fbRect(int x, int y, int w, int h, uint32_t color) {
	w += x;
	h += y;
	int _x = x;
	for (; y < h; y++) {
		for (x = _x; x < w; x++) {
			fbSet(x, y, color);
		}
	}
}

void fbFill(uint32_t color) {
	uint64_t len = framebufWidth * framebufHeight;
	for (uint64_t i = 0; i < len; i++) {
		framebuf[i] = color;
	}
}

void fbSet(int x, int y, uint32_t color) {
	if (x >= 0 && y >= 0 && x < framebufWidth && y < framebufHeight) {
		framebuf[x + y * framebufWidth] = color;
	}
}

uint32_t fbGet(int x, int y) {
	if (x >= 0 && y >= 0 && x < framebufWidth && y < framebufHeight) {
		return framebuf[x + y * framebufWidth];
	} else {
		return 0;
	}
}
