/*********************************************************************
This is a port of the Adafruit GFX library to Nordic nRF52832.

electronut.in
******************************************************************/

/******************************************************************
 This is the core graphics library for all our displays, providing
 basic graphics primitives (points, lines, circles, etc.). It needs
 to be paired with a hardware-specific library for each display
 device we carry (handling the lower-level functions).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source hardware
 by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries.
 BSD license, check license.txt for more information.
 All text above must be included in any redistribution.
 ******************************************************************/

#ifndef _ADAFRUIT_GFX_H
#define _ADAFRUIT_GFX_H

#include <stdint.h>
#include <stdbool.h>
#include "nordic_common.h"

#define swap(a, b) { int16_t t = a; a = b; b = t; }

int16_t  WIDTH, HEIGHT;   // this is the 'raw' display w/h - never changes
int16_t  _width, _height; // dependent on rotation
int16_t  cursor_x, cursor_y;
uint16_t textcolor, textbgcolor;
uint8_t  textsize;
uint8_t  rotation;
bool  wrap; // If set, 'wrap' text at right edge of display

// function pointer for drawPixels
typedef void (*DrawPixelHandler)(int16_t x, int16_t y, uint16_t color) ;
DrawPixelHandler _drawPixel;

// need to pass in the handler for drawing a pixel
void Adafruit_GFX_init(int16_t w, int16_t h, DrawPixelHandler drawPixel);

//
void Adafruit_GFX_drawPixel(int16_t x, int16_t y, uint16_t color);

void Adafruit_GFX_invertDisplay(bool i);

// these are 'generic' drawing functions, so we can share them!
 void Adafruit_GFX_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
	uint16_t color);
 void Adafruit_GFX_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
 void Adafruit_GFX_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
 void Adafruit_GFX_drawRect(int16_t x, int16_t y, int16_t w, int16_t h,
	uint16_t color);
 void Adafruit_GFX_fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
	uint16_t color);
 void Adafruit_GFX_fillScreen(uint16_t color);

void Adafruit_GFX_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void Adafruit_GFX_drawCircleHelper(int16_t x0, int16_t y0,
		int16_t r, uint8_t cornername, uint16_t color);
void Adafruit_GFX_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void Adafruit_GFX_fillCircleHelper(int16_t x0, int16_t y0, int16_t r,
	      uint8_t cornername, int16_t delta, uint16_t color);

void Adafruit_GFX_drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
	    int16_t x2, int16_t y2, uint16_t color);
void Adafruit_GFX_fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
	    int16_t x2, int16_t y2, uint16_t color);
void Adafruit_GFX_drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,
	     int16_t radius, uint16_t color);
void Adafruit_GFX_fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,
	     int16_t radius, uint16_t color);

void Adafruit_GFX_drawBitmap(int16_t x, int16_t y,
	  const uint8_t *bitmap, int16_t w, int16_t h,
	  uint16_t color);
void Adafruit_GFX_drawChar(int16_t x, int16_t y, unsigned char c,
	uint16_t color, uint16_t bg, uint8_t size);

void   Adafruit_GFX_write(uint8_t);

void Adafruit_GFX_setCursor(int16_t x, int16_t y);
void Adafruit_GFX_setTextColor(uint16_t c, uint16_t bg);
void Adafruit_GFX_setTextSize(uint8_t s);
void Adafruit_GFX_setTextWrap(bool w);

int16_t Adafruit_GFX_height(void);
int16_t Adafruit_GFX_width(void);

void Adafruit_GFX_setRotation(uint8_t r);
uint8_t Adafruit_GFX_getRotation(void);

#endif
