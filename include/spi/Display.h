#ifndef Display_H
#define Display_H

#include <Arduino.h>
#include "Arduino_GFX_Library.h"
#include "globals/pinout.h"

constexpr uint16_t ScreenWidth = 320;
constexpr uint16_t ScreenHeight = 240;

class Display {
public:
	Display() = default;
	static void initialize();
	static void test() { fillCheckerboard(RED, WHITE, 4); }
	static void fillCheckerboard(uint16_t color1, uint16_t color2, uint16_t scale);
	static void fillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
};

#endif