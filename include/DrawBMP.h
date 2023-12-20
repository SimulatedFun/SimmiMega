#ifndef DrawBMP_h
#define DrawBMP_h

#include <Arduino.h>
#include "spi/MicroSD.h"
#include "spi/Display.h"

void drawRGBBitmap(uint16_t x, uint16_t y, const uint16_t* bitmap, uint16_t w, uint16_t h);
sd_err_code bmpDraw(const String& filename, uint16_t x = 0, uint16_t y = 0, uint16_t maxWidth = 320,
						  uint16_t maxHeight = 240);
uint16_t read16(SDFile& f);
uint32_t read32(SDFile& f);

#endif