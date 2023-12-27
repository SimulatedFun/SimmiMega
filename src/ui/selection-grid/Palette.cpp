#include "ui/selection-grid/Palette.h"

void PaletteSelectionGrid::handlePress() {
	INFO(getRelativeX());
	INFO(getRelativeY());
	const uint8_t col = getRelativeX() / 78;
	const uint8_t row = getRelativeY() / 45;
	INFO("col: " << col << ", row: " << row);
	const uint8_t index = row * 4 + col;
	INFO(F("selected palette id: ") << index);
	callback.function(index);
}

void PaletteSelectionGrid::render() {
	Palette palette;

	for (uint8_t i = 0; i < palettesPerTab; i++) {
		palette.id = i + (*currentPage * palettesPerTab);
		palette.load();

		const uint16_t xPos = x + 1 + (i % 4) * 78;
		const uint16_t yPos = y + 1 + (i / 4) * 45;

		display->startWrite();
		{
			display->fillRectangleTx(xPos, yPos, 76, 14, palette.getHighlight());
			display->fillRectangleTx(xPos, yPos + 14, 76, 15, palette.getForeground());
			display->fillRectangleTx(xPos, yPos + 29, 76, 14, palette.getBackground());

			display->drawDottedRectangleTx(xPos, yPos, 75, 42, LIGHT_GREY);
		}
		display->endWrite();
	}
}