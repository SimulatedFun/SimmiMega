#ifndef XYSelectionGrid_h
#define XYSelectionGrid_h

#include <Arduino.h>
#include "spi/Display.h"
#include "DisplayExtra.h"
#include "ui/UIElement.h"
#include "RoomHelper.h"

class XYSelectionGrid : public UIElement {
	public:
		uint8_t roomId = _NO_ROOM;
		void render() override;
		void handlePress() override;
		XYSelectionGrid() {
			width = 312;
			height = 240;
		}
		~XYSelectionGrid() {
		}
		void setRoom(uint8_t inRoomId) {
			roomId = inRoomId;
		}
		Callback<Coordinates> callback;
};

#endif