#include "ui/selection-grid/Room.h"

/// Draws a minimap version of each room for the user to select from
void RoomSelectionGrid::render() {
	// todo check if the thumbnails exist and are valid to use
	char* preview = new char[roomStructSize];
	Palette* palette = new Palette();

	// for each room (64 total displayed in the room tray),
	// draw a preview of each room for the user to select from
	for (uint8_t rmId_x = 0; rmId_x < 6; rmId_x++) {
		for (uint8_t rmId_y = 0; rmId_y < 6; rmId_y++) {
			const uint8_t roomId = rmId_x + rmId_y * 6;
			const uint16_t xPos = (rmId_x * 13) * 3;
			const uint16_t yPos = (rmId_y * 10) * 3;

			RoomHelper::getPaletteId(&palette->id, roomId);
			palette->load();

			// reads one line of thumbnail data into the buffer
			microSd->getRoomThumbnailData(preview, roomId);

			// draws the room preview with a dashed outline
			display->startWrite();
			{
				for (uint8_t y = 0; y < 10; y++) {
					for (uint8_t x = 0; x < 13; x++) {
						const uint8_t roomIndex = y * 13 + x;
						const uint8_t colorIndex = preview[roomIndex] - '0';
						const uint16_t color = palette->getColorByIndex(colorIndex);
						display->fillRectangleTx(xPos + (x * 3), yPos + (y * 3), 3, 3, color);
					}
				}
				display->drawDottedRectangleTx(xPos, yPos, 38, 30, LIGHT_GREY); // outline
			}
			display->endWrite();
		}
	}

	delete palette;
	delete[] preview;
}

void RoomSelectionGrid::handlePress() {
	const uint8_t col = getRelativeX() / 40;
	const uint8_t row = getRelativeY() / 30;
	const uint8_t index = row * 6 + col;
	INFO(F("selected room id: ") << index);

	// cancel press if selected flag is OOB
	if (index >= flagCount) {
		return;
	}
	callback.function(index);
}