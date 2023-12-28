#include "DrawingUtils.h"

/// Fills the screen with a color
/// \param color The color to fill the screen with
void DrawingUtils::fill(u16 color) {
	display->fillRectangle(0, 0, 312, 240, color);
}

void DrawingUtils::dither(u16 color, boo dense) {
	display->startWrite();
	{
		for (uint16_t x = 0; x < (312 / 2); x++) {
			display->drawVerticalLineTx((int16_t) (x * 2), 0, 240, color);
		}
		if (dense) {
			for (uint16_t y = 0; y < (240 / 2); y++) {
				display->drawHorizontalLineTx(0, (int16_t) (y * 2), 312, color);
			}
		}
	}
	display->endWrite();
}

/// Draws 4x4 px checkerboard pattern alternating colors
/// \param color1 first rgb565 color to draw
/// \param color2 second rgb565 color to draw
void DrawingUtils::fillCheckerboard(u16 color1, u16 color2) {
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

void DrawingUtils::testRefreshRate() {
	for (uint8_t r = 0; r < 255; r += 85) {
		for (uint8_t g = 0; g < 255; g += 85) {
			for (uint8_t b = 0; b < 255; b += 85) {
				const uint16_t color = RGB565(r, g, b);
				DrawingUtils::fill(color);
			}
		}
	}
}

void DrawingUtils::simplePrint(u16 x, u16 y, str text, u16 color, u8 fontSize) {
	Text* t = new Text(text);
	t->setPosition(x, y);
	t->setStyle(fontSize, color);
	t->print();
	delete t;
}

void DrawingUtils::drawTitle(str title) {
	Text* text = new Text(title);
	text->setPosition(10, 28);
	text->setStyle(2, WHITE);
	text->setShadow(RGB565(0x2d1b2e), true);
	text->print();
	delete text;
}

void DrawingUtils::drawSplash() {
	INFO("drawing splash image");
	display->startWrite();
	{
		display->fillRectangleTx(312, 0, 8, 240, BLACK);
		display->fillRectangleTx(0, 0, 312, 240, RGB565(0x574b67));
		display->fillRectangleTx(40, 40, 232, 160, RGB565(0x6c5b83));
	}
	display->endWrite();
	drawRGBBitmap(58, 91, intro, 197, 43, 0xf81f);
	DrawingUtils::simplePrint(87, 150, "Micro Game-Making Machine", WHITE, 1);
	DrawingUtils::simplePrint(93, 225, "Made by Adam Kehl 2023", RGB565(0x909090), 1);
}
