#include "ui/selection-grid/Room.h"

/// Draws a minimap version of each room for the user to select from
void RoomSelectionGrid::render() {
	Palette* palette = new Palette();

	// for each room (64 total displayed in the room tray),
	// draw a preview of each room for the user to select from
	for (uint8_t rmId_x = 0; rmId_x < 6; rmId_x++) {
		for (uint8_t rmId_y = 0; rmId_y < 6; rmId_y++) {
			const uint8_t roomId = rmId_x + rmId_y * 6;
			const uint16_t xPos = (rmId_x * 13) * 3 + x;
			const uint16_t yPos = (rmId_y * 10) * 3 + y;

			uint8_t data[130] = {0};
			Coordinates coords;
			GameObject obj(0);
			uint8_t iterator = 0;

			for (uint8_t y = 0; y < 10; y++) {
				for (uint8_t x = 0; x < 13; x++) {
					coords = Coordinates(x, y, roomId);
					RoomHelper::getGameObjectId(&obj.id, coords);
					obj.load();

					uint8_t depth = PALETTE_BACKGROUND;
					if (countSetBits(obj.data.frame1) >= 32) {
						if (obj.isHighlighted()) {
							depth = PALETTE_HIGHLIGHT;
						} else {
							depth = PALETTE_FOREGROUND;
						}
					} else if (countSetBits(obj.data.frame1) >= 16) {
						depth = PALETTE_FOREGROUND;
					}
					data[iterator++] = depth;
				}
			}

			RoomHelper::getPaletteId(&palette->id, roomId);
			palette->load();

			// draws the room preview with a dashed outline
			display->startWrite();
			{
				for (uint8_t y = 0; y < 10; y++) {
					for (uint8_t x = 0; x < 13; x++) {
						const uint8_t roomIndex = y * 13 + x;
						const uint16_t color = palette->getColorByIndex(data[roomIndex]);
						display->fillRectangleTx(xPos + (x * 3), yPos + (y * 3), 3, 3, color);
					}
				}
				display->drawDottedRectangleTx(xPos, yPos, 38, 30, LIGHT_GREY); // outline
			}
			display->endWrite();
		}
	}

	delete palette;
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