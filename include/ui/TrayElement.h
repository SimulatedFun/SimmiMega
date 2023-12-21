#ifndef TrayElement_h
#define TrayElement_h

#include <Arduino.h>
#include "spi/Display.h"
#include "DisplayExtra.h"
#include "UIElement.h"
#include "RoundButton.h"
#include "RoomHelper.h"

/// Used in the sprite editor where the user edits a sprite
class SpriteEditGrid : public UIElement {
	public:
		void render() override {
			UIElement::drawOuterBorder(true);
		}
		void handlePress() override;
		SpriteEditGrid() {
			width = 232;
			height = 232;
			debounceDelayMS = 0;
		}
		~SpriteEditGrid() {
		}
		Callback<uint8_t> callback;
};
class RoomEditGrid : public UIElement {
	public:
		uint8_t roomId = 0;
		Palette* palette = nullptr;
		void render() override;
		void handlePress() override;
		RoomEditGrid() {
			width = 208;
			height = 160;
			debounceDelayMS = 0;
		}
		~RoomEditGrid() {
		}
		Callback<uint8_t> callback;

		void drawObjectInRoom(uint8_t index);
		void drawAllObjects();
};
const uint8_t objectsPerTrayRow = 19;
const uint8_t objectsPerTray = 57;
class RoomTraySelectGrid : public UIElement {
	public:
		uint16_t highlightedGameObjectId = 0;
		uint8_t currentTab = 0;
		Palette* palette = nullptr;
		void render() override;
		void handlePress() override;
		RoomTraySelectGrid() {
			width = 304;
			height = 48;
		}
		~RoomTraySelectGrid() {
		}
		Callback<boolean> callback;

		void drawObjectInTray(uint8_t index);
};
#endif