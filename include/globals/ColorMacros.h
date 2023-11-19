/// Some useful macros for handling converting HEX to rgb565 for the screen.

#ifndef ColorMacros_h
#define ColorMacros_h

#include <Arduino.h>

/// Converts hex color codes to RGB 565 at compile time
/// \param hex Hex formatted color code (e.g. 0x4d1250)
constexpr uint16_t RGB565(uint32_t hex) {
	return (((((hex & 0xFF0000) >> 16) >> 3) << 11) |
			  ((((hex & 0x00FF00) >> 8) >> 2) << 5) |
			  ((hex & 0x0000FF) >> 3));
}

constexpr uint16_t RGB565(uint8_t red, uint8_t green, uint8_t blue) {
	return ((red & 0xF8) << 8) |
			 ((green & 0xFC) << 3) |
			 (blue >> 3);
}

#endif