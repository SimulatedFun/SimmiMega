#ifndef DrawingUtils_h
#define DrawingUtils_h

#include <Arduino.h>
#include "spi/Display.h"
#include "globals/Glyphs.h"
#include "globals/ColorMacros.h"

namespace DrawingUtils {
	void fill(uint16_t color);
	void dither(uint16_t color, boolean dense = false);
	void fillCheckerboard(uint16_t color1, uint16_t color2);
	void testRefreshRate();
} // namespace DrawingUtils

#endif