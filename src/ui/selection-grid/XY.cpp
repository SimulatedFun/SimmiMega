#include "ui/selection-grid/XY.h"

void XYSelectionGrid::render() {
	GameObject gameObject(0);

	Palette palette;
	RoomHelper::getPaletteId(&palette.id, roomId);
	palette.load();

	Coordinates coordinates;
	coordinates.roomId = roomId;

	touch->clearQueue();

	// Draws the room under the xy selector ui
	for (uint8_t y = 0; y < 10; y++) {
		coordinates.y = y;
		for (uint8_t x = 0; x < 13; x++) {
			coordinates.x = x;
			RoomHelper::getGameObjectId(&gameObject.id, coordinates);
			gameObject.load();
			drawGameObjectAtCoords(&gameObject, &palette, &coordinates, 3, first_frame, true);
		}
	}
}

void XYSelectionGrid::handlePress() {
	const tsPoint_t pt = getRelativePoint();
	const uint8_t col = pt.x / 24;
	const uint8_t row = pt.y / 24;
	const Coordinates coords = Coordinates(col, row, _NO_ROOM);
	callback.function(coords);
}
