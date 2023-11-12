#include "spi/Display.h"

Arduino_ESP32SPI bus = Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCK, TFT_MOSI, TFT_MISO);
Arduino_ILI9341 display = Arduino_ILI9341(&bus, TFT_RESET);

void Display::initialize() {
	display.begin();
	display.setRotation(1);
	Display::fillRectangle(312, 0, 8, 240, BLACK); // rightmost black edge
	ESP_LOGV(Info, "display initialized");
}

void Display::fillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
	display.fillRect((int16_t)x, (int16_t)y, (int16_t)w, (int16_t)h, color);
}

/// Draws 4x4 px checkerboard pattern alternating colors
/// \param color1 first rgb565 color to draw
/// \param color2 second rgb565 color to draw
void Display::fillCheckerboard(uint16_t color1, uint16_t color2, uint16_t scale) {
	if (ScreenHeight % scale != 0 or ScreenWidth % scale != 0) {
		// only run for divisible examples
		return;
	}
	boolean i = false;
	for (uint16_t x = 0; x < ScreenWidth / scale; x++) {
		for (uint16_t y = 0; y < ScreenHeight / scale; y++) {
			if (i) {
				Display::fillRectangle(x * scale, y * scale, scale, scale, color1);
			} else {
				Display::fillRectangle(x * scale, y * scale, scale, scale, color2);
			}
			i = !i;
		}
		i = !i;
	}
}