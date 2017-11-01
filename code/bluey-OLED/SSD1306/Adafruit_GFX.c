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

#include <stdlib.h>

#include "Adafruit_GFX.h"
#include "glcdfont.c"

#define pgm_read_byte(addr) (*(const unsigned char *)(addr))

void Adafruit_GFX_init(int16_t w, int16_t h, DrawPixelHandler drawPixel) {
  _width = WIDTH = w;
  _height = HEIGHT = h;
  _drawPixel = drawPixel;

  rotation = 0;
  cursor_y = cursor_x = 0;
  textsize = 1;
  textcolor = textbgcolor = 0xFFFF;
  wrap = true;
}


// draw a circle outline
void Adafruit_GFX_drawCircle(int16_t x0, int16_t y0, int16_t r,
			      uint16_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  Adafruit_GFX_drawPixel(x0, y0+r, color);
  Adafruit_GFX_drawPixel(x0, y0-r, color);
  Adafruit_GFX_drawPixel(x0+r, y0, color);
  Adafruit_GFX_drawPixel(x0-r, y0, color);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    Adafruit_GFX_drawPixel(x0 + x, y0 + y, color);
    Adafruit_GFX_drawPixel(x0 - x, y0 + y, color);
    Adafruit_GFX_drawPixel(x0 + x, y0 - y, color);
    Adafruit_GFX_drawPixel(x0 - x, y0 - y, color);
    Adafruit_GFX_drawPixel(x0 + y, y0 + x, color);
    Adafruit_GFX_drawPixel(x0 - y, y0 + x, color);
    Adafruit_GFX_drawPixel(x0 + y, y0 - x, color);
    Adafruit_GFX_drawPixel(x0 - y, y0 - x, color);

  }
}

void Adafruit_GFX_drawCircleHelper( int16_t x0, int16_t y0,
               int16_t r, uint8_t cornername, uint16_t color) {
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    if (cornername & 0x4) {
      Adafruit_GFX_drawPixel(x0 + x, y0 + y, color);
      Adafruit_GFX_drawPixel(x0 + y, y0 + x, color);
    }
    if (cornername & 0x2) {
      Adafruit_GFX_drawPixel(x0 + x, y0 - y, color);
      Adafruit_GFX_drawPixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8) {
      Adafruit_GFX_drawPixel(x0 - y, y0 + x, color);
      Adafruit_GFX_drawPixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1) {
      Adafruit_GFX_drawPixel(x0 - y, y0 - x, color);
      Adafruit_GFX_drawPixel(x0 - x, y0 - y, color);
    }
  }
}

void Adafruit_GFX_fillCircle(int16_t x0, int16_t y0, int16_t r,
			      uint16_t color) {
  Adafruit_GFX_drawFastVLine(x0, y0-r, 2*r+1, color);
  Adafruit_GFX_fillCircleHelper(x0, y0, r, 3, 0, color);
}

// used to do circles and roundrects!
void Adafruit_GFX_fillCircleHelper(int16_t x0, int16_t y0, int16_t r,
				    uint8_t cornername, int16_t delta, uint16_t color) {

  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;

    if (cornername & 0x1) {
      Adafruit_GFX_drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
      Adafruit_GFX_drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
    }
    if (cornername & 0x2) {
      Adafruit_GFX_drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
      Adafruit_GFX_drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
    }
  }
}

// bresenham's algorithm - thx wikpedia
void Adafruit_GFX_drawLine(int16_t x0, int16_t y0,
			    int16_t x1, int16_t y1,
			    uint16_t color) {
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0<=x1; x0++) {
    if (steep) {
      Adafruit_GFX_drawPixel(y0, x0, color);
    } else {
      Adafruit_GFX_drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}


// draw a rectangle
void Adafruit_GFX_drawRect(int16_t x, int16_t y,
			    int16_t w, int16_t h,
			    uint16_t color) {
  Adafruit_GFX_drawFastHLine(x, y, w, color);
  Adafruit_GFX_drawFastHLine(x, y+h-1, w, color);
  Adafruit_GFX_drawFastVLine(x, y, h, color);
  Adafruit_GFX_drawFastVLine(x+w-1, y, h, color);
}

void Adafruit_GFX_drawFastVLine(int16_t x, int16_t y,
				 int16_t h, uint16_t color) {
  // stupidest version - update in subclasses if desired!
  Adafruit_GFX_drawLine(x, y, x, y+h-1, color);
}


void Adafruit_GFX_drawFastHLine(int16_t x, int16_t y,
				 int16_t w, uint16_t color) {
  // stupidest version - update in subclasses if desired!
  Adafruit_GFX_drawLine(x, y, x+w-1, y, color);
}

void Adafruit_GFX_fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
			    uint16_t color) {
  // stupidest version - update in subclasses if desired!
  for (int16_t i=x; i<x+w; i++) {
    Adafruit_GFX_drawFastVLine(i, y, h, color);
  }
}


void Adafruit_GFX_fillScreen(uint16_t color) {
  Adafruit_GFX_fillRect(0, 0, _width, _height, color);
}

// draw a rounded rectangle!
void Adafruit_GFX_drawRoundRect(int16_t x, int16_t y, int16_t w,
  int16_t h, int16_t r, uint16_t color) {
  // smarter version
  Adafruit_GFX_drawFastHLine(x+r  , y    , w-2*r, color); // Top
  Adafruit_GFX_drawFastHLine(x+r  , y+h-1, w-2*r, color); // Bottom
  Adafruit_GFX_drawFastVLine(  x    , y+r  , h-2*r, color); // Left
  Adafruit_GFX_drawFastVLine(  x+w-1, y+r  , h-2*r, color); // Right
  // draw four corners
  Adafruit_GFX_drawCircleHelper(x+r    , y+r    , r, 1, color);
  Adafruit_GFX_drawCircleHelper(x+w-r-1, y+r    , r, 2, color);
  Adafruit_GFX_drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
  Adafruit_GFX_drawCircleHelper(x+r    , y+h-r-1, r, 8, color);
}

// fill a rounded rectangle!
void Adafruit_GFX_fillRoundRect(int16_t x, int16_t y, int16_t w,
				 int16_t h, int16_t r, uint16_t color) {
  // smarter version
  Adafruit_GFX_fillRect(x+r, y, w-2*r, h, color);

  // draw four corners
  Adafruit_GFX_fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
  Adafruit_GFX_fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
}

// draw a triangle!
void Adafruit_GFX_drawTriangle(int16_t x0, int16_t y0,
				int16_t x1, int16_t y1,
				int16_t x2, int16_t y2, uint16_t color) {
  Adafruit_GFX_drawLine(x0, y0, x1, y1, color);
  Adafruit_GFX_drawLine(x1, y1, x2, y2, color);
  Adafruit_GFX_drawLine(x2, y2, x0, y0, color);
}

// fill a triangle!
void Adafruit_GFX_fillTriangle ( int16_t x0, int16_t y0,
				  int16_t x1, int16_t y1,
				  int16_t x2, int16_t y2, uint16_t color) {

  int16_t a, b, y, last;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) {
    swap(y0, y1); swap(x0, x1);
  }
  if (y1 > y2) {
    swap(y2, y1); swap(x2, x1);
  }
  if (y0 > y1) {
    swap(y0, y1); swap(x0, x1);
  }

  if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if(x1 < a)      a = x1;
    else if(x1 > b) b = x1;
    if(x2 < a)      a = x2;
    else if(x2 > b) b = x2;
    Adafruit_GFX_drawFastHLine(a, y0, b-a+1, color);
    return;
  }

  int16_t
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1,
    sa   = 0,
    sb   = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if(y1 == y2) last = y1;   // Include y1 scanline
  else         last = y1-1; // Skip it

  for(y=y0; y<=last; y++) {
    a   = x0 + sa / dy01;
    b   = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;
    /* longhand:
    a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if(a > b) swap(a,b);
    Adafruit_GFX_drawFastHLine(a, y, b-a+1, color);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);
  for(; y<=y2; y++) {
    a   = x1 + sa / dy12;
    b   = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;
    /* longhand:
    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if(a > b) swap(a,b);
    Adafruit_GFX_drawFastHLine(a, y, b-a+1, color);
  }
}

void Adafruit_GFX_drawBitmap(int16_t x, int16_t y,
			      const uint8_t *bitmap, int16_t w, int16_t h,
			      uint16_t color) {

  int16_t i, j, byteWidth = (w + 7) / 8;

  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
	Adafruit_GFX_drawPixel(x+i, y+j, color);
      }
    }
  }
}


void Adafruit_GFX_write(uint8_t c) {
  if (c == '\n') {
    cursor_y += textsize*8;
    cursor_x = 0;
  } else if (c == '\r') {
    // skip em
  } else {
    Adafruit_GFX_drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
    cursor_x += textsize*6;
    if (wrap && (cursor_x > (_width - textsize*6))) {
      cursor_y += textsize*8;
      cursor_x = 0;
    }
  }

}

// draw a character
void Adafruit_GFX_drawChar(int16_t x, int16_t y, unsigned char c,
			    uint16_t color, uint16_t bg, uint8_t size) {

  if((x >= _width)            || // Clip right
     (y >= _height)           || // Clip bottom
     ((x + 5 * size - 1) < 0) || // Clip left
     ((y + 8 * size - 1) < 0))   // Clip top
    return;

  for (int8_t i=0; i<6; i++ ) {
    uint8_t line;
    if (i == 5)
      line = 0x0;
    else
      line = pgm_read_byte(font+(c*5)+i);
    for (int8_t j = 0; j<8; j++) {
      if (line & 0x1) {
        if (size == 1) // default size
          Adafruit_GFX_drawPixel(x+i, y+j, color);
        else {  // big size
          Adafruit_GFX_fillRect(x+(i*size), y+(j*size), size, size, color);
        }
      } else if (bg != color) {
        if (size == 1) // default size
          Adafruit_GFX_drawPixel(x+i, y+j, bg);
        else {  // big size
          Adafruit_GFX_fillRect(x+i*size, y+j*size, size, size, bg);
        }
      }
      line >>= 1;
    }
  }
}

void Adafruit_GFX_setCursor(int16_t x, int16_t y) {
  cursor_x = x;
  cursor_y = y;
}


void Adafruit_GFX_setTextSize(uint8_t s) {
  textsize = (s > 0) ? s : 1;
}



 void Adafruit_GFX_setTextColor(uint16_t c, uint16_t b) {
   textcolor = c;
   textbgcolor = b;
 }

void Adafruit_GFX_setTextWrap(bool w) {
  wrap = w;
}

uint8_t Adafruit_GFX_getRotation(void) {
  rotation %= 4;
  return rotation;
}

void Adafruit_GFX_setRotation(uint8_t x) {
  x %= 4;  // cant be higher than 3
  rotation = x;
  switch (x) {
  case 0:
  case 2:
    _width = WIDTH;
    _height = HEIGHT;
    break;
  case 1:
  case 3:
    _width = HEIGHT;
    _height = WIDTH;
    break;
  }
}

void Adafruit_GFX_invertDisplay(bool i) {
  // do nothing, can be subclassed
}


// return the size of the display which depends on the rotation!
int16_t Adafruit_GFX_width(void) {
  return _width;
}

int16_t Adafruit_GFX_height(void) {
  return _height;
}

void Adafruit_GFX_drawPixel(int16_t x, int16_t y, uint16_t color) {
  if(_drawPixel) {
    _drawPixel(x, y, color);
  }
}
