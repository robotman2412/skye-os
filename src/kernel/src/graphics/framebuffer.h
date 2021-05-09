
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#include <stdint.h>
#include "../memory.h"

#define COLOR_BLACK			0x00000000
#define COLOR_DARK_RED		0xff7f0000
#define COLOR_DARK_GREEN	0xff007f00
#define COLOR_DARK_YELLOW	0xff7f3f00
#define COLOR_DARK_BLUE		0xff00007f
#define COLOR_DARK_MAGENTA	0xff7f007f
#define COLOR_DARK_CYAN		0xff007f7f
#define COLOR_LIGHT_GRAY	0xff9f9f9f
#define COLOR_DARK_GRAY		0xff7f7f7f
#define COLOR_RED			0xffff3f3f
#define COLOR_GREEN			0xff3fff3f
#define COLOR_YELLOW		0xffffff3f
#define COLOR_BLUE			0xff3f3fff
#define COLOR_MAGENTA		0xffff3fff
#define COLOR_CYAN			0xff3fffff
#define COLOR_WHITE			0xffffffff

extern uint32_t ansiColors[16];

// Variables.
extern uint32_t* framebuf;
extern uint64_t framebufWidth;
extern uint64_t framebufHeight;
extern char doubleTextSize;

// TTY variables.
extern uint32_t ttyFgCol;
extern uint32_t ttyBgCol;
extern int ttyMaxX;
extern int ttyMaxY;
extern int ttyXPos;
extern int ttyYPos;

// Set-up for the FRBBFBFBFBF
void fbSetup();
void fbDrawIcon();
// Prints text in a bit of TTY action.
void fbPrint(char *text);
void fbPutc(char text);
void fbPuthex(uint64_t val, int num);
// TTY newline-ables.
void fbNewln();
// Draws text.
void fbText(int x, int y, char *text, uint32_t color);
void fbTextb(int x, int y, char *text, uint32_t color, uint32_t bgColor);
// Draws a character.
void fbChar(int x, int y, char text, uint32_t color, uint32_t bgColor);
// Fills the entire screen.
void fbFill(uint32_t color);
// Fills a rectangle.
void fbRect(int x, int y, int w, int h, uint32_t color);
// Sets a single pixel.
void fbSet(int x, int y, uint32_t color);
// Gets a single pixel.
uint32_t fbGet(int x, int y);

#endif //FRAMEBUFFER_H
