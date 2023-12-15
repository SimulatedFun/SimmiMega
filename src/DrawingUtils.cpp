#include "DrawingUtils.h"

namespace DrawingUtils {

	/// Fills the screen with a color
	/// \param color The color to fill the screen with
	void fill(uint16_t color) {
		display->fillRectangle(0, 0, 312, 240, color);
	}

	void dither(uint16_t color, boolean dense) {
		display->startWrite();
		{
			for (uint16_t x = 0; x < (312 / 2); x++) {
				display->drawVerticalLineTx(x * 2, 0, 240, color);
			}
			if (dense) {
				for (uint16_t y = 0; y < (240 / 2); y++) {
					display->drawHorizontalLineTx(0, y * 2, 312, color);
				}
			}
		}
		display->endWrite();
	}

	/// Draws 4x4 px checkerboard pattern alternating colors
	/// \param color1 first rgb565 color to draw
	/// \param color2 second rgb565 color to draw
	void fillCheckerboard(uint16_t color1, uint16_t color2) {
		boolean i = false;
		for (uint16_t x = 0; x < ScreenWidth / 4; x++) {
			for (uint16_t y = 0; y < ScreenHeight / 4; y++) {
				if (i) {
					display->fillRectangle(x * 4, y * 4, 4, 4, color1);
				} else {
					display->fillRectangle(x * 4, y * 4, 4, 4, color2);
				}
				i = !i;
			}
			i = !i;
		}
	}

	void testRefreshRate() {
		for (uint8_t r = 0; r < 255; r += 85) {
			for (uint8_t g = 0; g < 255; g += 85) {
				for (uint8_t b = 0; b < 255; b += 85) {
					const uint16_t color = RGB565(r, g, b);
					DrawingUtils::fill(color);
				}
			}
		}
	}
} // namespace DrawingUtils