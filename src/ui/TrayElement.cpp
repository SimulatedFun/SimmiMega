#include "ui/TrayElement.h"

void SpriteEditGrid::handlePress() {
	const uint8_t col = getRelativeX() / 29;
	const uint8_t row = getRelativeY() / 29;
	const uint8_t index = row * 8 + col; // 0 through 64
	DEBUG(F("draw pixel index: ") << index);
	callback.function(index);
}

void RoomEditGrid::handlePress() {
	const uint8_t col = getRelativeX() / 16;
	const uint8_t row = getRelativeY() / 16;
	const uint8_t index = row * 13 + col;
	DEBUG(F("room index: ") << index);
	callback.function(index);
}

void RoomTraySelectGrid::render() {
	UIElement::drawOuterBorder(true);
	for (uint8_t i = 0; i < objectsPerTray; i++) {
		drawObjectInTray(i);
	}
}

void RoomTraySelectGrid::drawObjectInTray(uint8_t index) {
	const uint16_t x = (index % objectsPerTrayRow) * 16 + 4;
	const uint16_t y = (index / objectsPerTrayRow) * 16 + 169;
	const uint16_t gameObjectId = index + (currentTab * objectsPerTray);

    // don't try and draw any objects OOB
    // draw plain bg instead
    if (gameObjectId >= objectCount) {
        display->fillRectangle(x, y, 16, 16, RGB565(0x2d1b2e));
        return;
    }

	GameObject obj(gameObjectId);
	obj.load();
	drawGameObject(&obj, palette, x, y, 2, first_frame, true);
}

void RoomTraySelectGrid::handlePress() {
	const uint8_t col = getRelativeX() / 16;
	const uint8_t row = getRelativeY() / 16;
	const uint8_t index = row * objectsPerTrayRow + col;
    const uint16_t gameObjectId = index + (currentTab * objectsPerTray);

    // don't allow the user to select a game object OOB
    if (gameObjectId >= objectCount) {
        return;
    }

	// un-draw prior selection
	drawObjectInTray(highlightedGameObjectId - (currentTab * objectsPerTray));

	highlightedGameObjectId = gameObjectId;
	INFO(F("selected obj from tray: ") << highlightedGameObjectId);
	if (callback.function != nullptr) {
		callback.function(true);
	}
}

void RoomEditGrid::render() {
	UIElement::drawOuterBorder(true);
	drawAllObjects();
}

void RoomEditGrid::drawAllObjects() {
	for (uint8_t i = 0; i < objectsPerRoom; i++) {
		drawObjectInRoom(i);
	}
}

void RoomEditGrid::drawObjectInRoom(uint8_t index) {
	const uint16_t x = index % 13;
	const uint16_t y = index / 13;
	const Coordinates coords(x, y, roomId);
	GameObject gameObject(0);
	RoomHelper::getGameObjectId(&gameObject.id, coords);
	gameObject.load();
	drawGameObject(&gameObject, palette, 4 + x * 16, 4 + y * 16, 2, first_frame, true);
}