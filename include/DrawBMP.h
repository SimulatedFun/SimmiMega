#ifndef DrawBMP_h
#define DrawBMP_h

#include <Arduino.h>
#include "globals/Typedefs.h"
#include "spi/MicroSD.h"
#include "spi/Display.h"

void drawRGBBitmap(u16 x, u16 y, const u16* bitmap, u16 w, u16 h);
sd_err bmpDraw(str filename, u16 x = 0, u16 y = 0, u16 maxWidth = 320, u16 maxHeight = 240);
u16 read16(SDFile& f);
u32 read32(SDFile& f);

#endif