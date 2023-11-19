#ifndef Display_H
#define Display_H

#include <Arduino.h>
#include "Arduino_GFX_Library.h"
#include "globals/Pinout.h"
#include "globals/Colors.h"

constexpr uint16_t ScreenWidth = 320;
constexpr uint16_t ScreenHeight = 240;

class Display {
	public:
		Display() = default;
		static void initialize();

		static void startWrite();
		static void endWrite();

		static void writeColor(uint16_t color, uint32_t len);

		static void fillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
		static void fillRectangleTx(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
		static void drawPixelTx(uint16_t x, uint16_t y, uint16_t color);
		static void drawHorizontalLineTx(uint16_t x, uint16_t y, uint16_t w, uint16_t color);
		static void drawVerticalLineTx(uint16_t x, uint16_t y, uint16_t h, uint16_t color);
		static void setAddrWindow(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h);
		static void drawDottedRectangleTx(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
													 uint16_t color);
		static void drawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
		static void drawRectangleTx(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
		static void drawDottedRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
												  uint16_t color);
};

#endif