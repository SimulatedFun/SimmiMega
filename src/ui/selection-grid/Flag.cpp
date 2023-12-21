#include "ui/selection-grid/Flag.h"

void FlagSelectionGrid::render() {
	const uint16_t lightSquare = RGB565(0xd2d2d2);
	const uint16_t darkSquare = RGB565(0x9a9a9a);

	/// Draws a tray of flags (12 col x 7 row)
	for (uint8_t y = 0; y < 7; y++) {
		for (uint8_t x = 0; x < 12; x++) {
			const uint16_t xPos = 1 + (x * 26);
			const uint16_t yPos = 35 + 1 + (y * 26);
			uint16_t id = y * 12 + x;
			id += (*currentPage) * flagsPerTab;

			// limit the draw of flags past bounds
			if (id == flagCount - 1) {
				return;
			}

			// draw alternating gray squares
			uint16_t color = lightSquare;
			if ((x % 2 == 1 and y % 2 == 1) or (x % 2 == 0)) {
				color = darkSquare;
			}
			display->fillRectangle(xPos, yPos, 24, 24, color);

			// draw purple flag ID in the gray square
			Text* text = new Text(id);
			text->setPosition(xPos + text->getPaddingToCenter(24), yPos + 15);
			text->setStyle(1, RGB565(0x574b67));
			text->print();
			delete text;
		}
	}
}

void FlagSelectionGrid::handlePress() {
	const uint8_t col = getRelativeX() / 26; // 0 through 11
	const uint8_t row = getRelativeY() / 26; // 0 through 6
	INFO("col: " << col << ", row: " << row);
	const uint8_t index = row * 12 + col;
	INFO(F("selected flag id: ") << index);
	callback.function(index);
}