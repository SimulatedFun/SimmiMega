#ifndef Palette_h
#define Palette_h

#include <Arduino.h>
#include "globals/DataBounds.h"
#include "spi/ExtEeprom.h"
#include "RamBlock.h"
#include "globals/Colors.h"

constexpr uint8_t PALETTE_HIGHLIGHT = 2;
constexpr uint8_t PALETTE_FOREGROUND = 1;
constexpr uint8_t PALETTE_BACKGROUND = 0;

struct Color {
		uint16_t highlight, foreground, background;
};

class Palette {
	private:
		Color color;
	public:
		uint8_t id = 0;

		Palette() {}
		Palette(uint16_t inHighlight, uint16_t inForeground, uint16_t inBackground) {
			color.highlight = inHighlight;
			color.foreground = inForeground;
			color.background = inBackground;
		}
		void load(boolean fromRam = true);
		void save();
		void setColorByIndex(uint8_t index, uint16_t inColor) {
			switch (index) {
				case PALETTE_HIGHLIGHT:
					color.highlight = inColor;
					return;
				case PALETTE_FOREGROUND:
					color.foreground = inColor;
					return;
				case PALETTE_BACKGROUND:
					color.background = inColor;
					return;
			}
		}
		uint16_t getColorByIndex(uint8_t index) {
			// clang-format off
			switch (index) {
				case PALETTE_HIGHLIGHT: return color.highlight;
				case PALETTE_FOREGROUND: return color.foreground;
				case PALETTE_BACKGROUND: return color.background;
				default: return PINK_COLOR;
			}
			// clang-format on
		}
		uint16_t getHighlight() {
			return color.highlight;
		}
		uint16_t getForeground() {
			return color.foreground;
		}
		uint16_t getBackground() {
			return color.background;
		}
		void toString() {
			Serial.printf("highlight: %d\n", getHighlight());
			Serial.printf("foreground: %d\n", getForeground());
			Serial.printf("background: %d\n", getBackground());
		}
};

#endif