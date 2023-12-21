#include "ui/windows/ChooseXY.h"

namespace ChooseXY {
	XYSelectionGrid* tray = nullptr;

	boolean callbackSelected = false;
	Coordinates callbackCoords;
	uint8_t roomId = _NO_ROOM;

	void callbackXYTray(Coordinates inCoords) {
		callbackSelected = true;
		callbackCoords = inCoords;
	}

	void setup() {
		setupUI();
		draw();
		touch->clearQueue();
		callbackSelected = false;
	}

	Coordinates pick(uint8_t inRoomId) {
		roomId = inRoomId;
		setup();

		while (!callbackSelected) {
			UIHelper::loop();
		}

		deallocate();
		INFO(F("selected XY: (") << callbackCoords.x << ", " << callbackCoords.y << ")");
		LEAK("returning from ChooseXY::pick()");
		touch->clearQueue();
		return callbackCoords;
	}

	void draw() {
		UIHelper::render();
	}

	void setupUI() {
		UIHelper::clearActive();

		tray = new XYSelectionGrid();
		tray->setPosition(0, 0);
		tray->setRoom(roomId);
		tray->callback.bind(&callbackXYTray);
		UIHelper::registerUI(tray);
	}

	void deallocate() {
		UIHelper::clearActive();
		delete tray;
	}
} // namespace ChooseXY