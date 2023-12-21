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
	const uint16_t x = index % objectsPerTrayRow;
	const uint16_t y = index / objectsPerTrayRow;
	const uint16_t gameObjectId = index + (currentTab * objectsPerTray);

	GameObject obj(gameObjectId);
	obj.load();
	drawGameObject(&obj, palette, 4 + x * 16, 169 + y * 16, 2, first_frame, true);
}

void RoomTraySelectGrid::handlePress() {
	const uint8_t col = getRelativeX() / 16;
	const uint8_t row = getRelativeY() / 16;
	const uint8_t index = row * objectsPerTrayRow + col;
	// un-draw prior selection
	drawObjectInTray(highlightedGameObjectId - (currentTab * objectsPerTray));

	const uint16_t gameObjectId = index + (currentTab * objectsPerTray);
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