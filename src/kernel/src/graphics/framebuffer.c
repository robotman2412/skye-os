
#include "../kernel.h"
#include "../memory.h"
#include "framebuffer.h"
#include "font7x9.h"
#include "icon.h"
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
static uint32_t *altBuf = 0;

uint64_t sisqrt(uint64_t val) {
	uint64_t est = 1;
	uint64_t n = 20;
	uint64_t inc = 1 << 12;
	char dir = 1;
	while (--n) {
		uint64_t g = est * est;
		if (g < val) {
			if (!dir) inc >>= 1;
			est += inc;
			dir = 1;
		} else if (g > val) {
			if (dir) inc >>= 1;
			est -= inc;
			dir = 0;
		} else {
			return est;
		}
	}
	return est;
}

void fbDrawIcon() {
	if (altBuf) {
		for (uint16_t y = 0; y < ICON_HEIGHT; y++) {
			for (uint16_t x = 0; x < ICON_WIDTH; x++) {
				uint8_t blend = iconData[x + y * ICON_WIDTH];
				uint8_t iblend = 255 - iconData[x + y * ICON_WIDTH];
				uint32_t alt = altBuf[framebufWidth - ICON_WIDTH + x + y * framebufWidth];
				uint8_t _red = (alt >> 16) & 0xff;
				uint8_t _grn = (alt >>  8) & 0xff;
				uint8_t _blu = (alt >>  0) & 0xff;
				uint8_t red = 0x85 * blend / 255 + _red * iblend / 255;
				uint8_t grn =        blend       + _grn * iblend / 255;
				uint8_t blu =        blend       + _blu * iblend / 255;
				uint32_t col = red << 16 | grn << 8 | blu;
				fbSet(framebufWidth - ICON_WIDTH + x, y, col);
			}
		}
	} else {
		for (uint16_t y = 0; y < ICON_HEIGHT; y++) {
			for (uint16_t x = 0; x < ICON_WIDTH; x++) {
				uint32_t col = iconData[x + y * ICON_WIDTH] * 0x010101 & 0x85ffff;
				fbSet(framebufWidth - ICON_WIDTH + x, y, col);
			}
		}
	}
}

void fbSetup() {
	struct pmm_entry *got = pmm_alloc(framebufWidth * framebufHeight * sizeof(uint32_t));
	altBuf = (uint32_t *) got->base;
	
	//Let's have some fun.
	for (uint16_t y = 0; y < framebufHeight; y ++) {
		for (uint16_t x = 0; x < framebufWidth; x ++) {
			uint32_t color = 0;
			uint16_t _x = framebufWidth - x - 1;
			uint16_t _y = framebufHeight - y - 1;
			uint8_t red = 255 - sisqrt(x * x + y * y) * 192 / framebufWidth;
			int16_t _green = sisqrt(_x * _x + y * y) * 400 / framebufWidth;
			uint8_t green = _green > 255 ? 511-_green : _green;
			uint8_t blue = 255 - (_x + _y) * 128 / framebufWidth;
			color = red << 16 | green << 8 | blue;
			//color >>= 1;
			//color &= 0x7f7f7f;
			color &= 0xcfcfcf;
			//color = (color >> 1 & 0x7f7f7f) + (color >> 2 & 0x3f3f3f);
			altBuf[x + y * framebufWidth] = color;
		}
	}
	
	fbMagicRect(0, 0, framebufWidth, framebufHeight);
	fbDrawIcon();
}

void fbPrint(char *text) {
	while (*text) {
		fbPutc(*text);
		text ++;
	}
}

void fbMagicRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h) {
	if (!altBuf) return;
	for (uint16_t y = y0; y < y0 + h; y ++) {
		for (uint16_t x = x0; x < x0 + w; x ++) {
			if (fbGet(x, y) & 0xffffff != 0) continue;
			fbSet(x, y, altBuf[x + y * framebufWidth]);
		}
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
	if (ttyYPos >= ttyMaxY) {
		int dest = 0;
		int numScrolled = 2;
		int src = framebufWidth * (doubleTextSize ? FONT_HEIGHT2 : FONT_HEIGHT) * numScrolled;
		int len = framebufWidth * framebufHeight - src;
		if (altBuf) {
			for (int i = 0; i < len; i++) {
				uint32_t val = framebuf[src + i];
				if (val & 0xff000000) {
					framebuf[dest + i] = val;
				} else {
					framebuf[dest + i] = altBuf[dest + i];
				}
			}
		} else {
			for (int i = 0; i < len; i++) {
				uint32_t val = framebuf[src + i];
				framebuf[dest + i] = val;
			}
		}
		ttyYPos = ttyMaxY - numScrolled;
		if (altBuf) {
			for (int i = len; i < framebufWidth * framebufHeight; i++) {
				uint32_t val = altBuf[dest + i];
				framebuf[dest + i] = val;
			}
		} else {
			for (int i = len; i < framebufWidth * framebufHeight; i++) {
				framebuf[dest + i] = ttyBgCol;
			}
		}
		fbDrawIcon();
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
		fbMagicRect(x, y, FONT_WIDTH2, FONT_HEIGHT2);
	} else {
		for (int _y = 0; _y < 9; _y ++) {
			for (int _x = 0; _x < 7; _x ++) {
				char val = glyphs[_y + offs] & (1 << _x);
				fbSet(x + _x, y + _y, val ? color : bgColor);
			}
		}
		fbMagicRect(x, y, FONT_WIDTH, FONT_HEIGHT);
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
