#include "DrawingUtils.h"

namespace DrawingUtils {

	/// Fills the screen with a color
	/// \param color The color to fill the screen with
	void fill(uint16_t color) {
		Display::fillRectangle(0, 0, 312, 240, color);
	}

	void dither(uint16_t color, boolean dense) {
		Display::startWrite();
		{
			for (uint16_t x = 0; x < (312 / 2); x++) {
				Display::drawVerticalLineTx(x * 2, 0, 240, color);
			}
			if (dense) {
				for (uint16_t y = 0; y < (240 / 2); y++) {
					Display::drawHorizontalLineTx(0, y * 2, 312, color);
				}
			}
		}
		Display::endWrite();
	}

	/// Draws 4x4 px checkerboard pattern alternating colors
	/// \param color1 first rgb565 color to draw
	/// \param color2 second rgb565 color to draw
	void fillCheckerboard(uint16_t color1, uint16_t color2) {
		boolean i = false;
		for (uint16_t x = 0; x < ScreenWidth / 4; x++) {
			for (uint16_t y = 0; y < ScreenHeight / 4; y++) {
				if (i) {
					Display::fillRectangle(x * 4, y * 4, 4, 4, color1);
				} else {
					Display::fillRectangle(x * 4, y * 4, 4, 4, color2);
				}
				i = !i;
			}
			i = !i;
		}
	}
} // namespace drawing_utils