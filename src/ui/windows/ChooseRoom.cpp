#include "ui/windows/ChooseRoom.h"

namespace ChooseRoom {
	RoomSelectionGrid* tray;
	RoundButton* exit;

	boolean callbackSelected = false;
	uint16_t callbackRoomId = _NO_ROOM;

	void callbackRoomTray(unsigned int selectedRoomId) {
		callbackSelected = true;
		callbackRoomId = selectedRoomId;
	}

	void callbackExit(RoundButton&) {
		callbackSelected = true;
		callbackRoomId = _NO_ROOM;
	}

	void setup() {
		setupUI();
		touch->clearQueue();
		draw();
		callbackSelected = false;
	}

	uint8_t pick() {
		setup();

		while (!callbackSelected) {
			UIHelper::loop();
		}

		deallocate();
		INFO(F("selected room: ") << callbackRoomId);
		LEAK("returning from ChooseRoom::pick()");
		touch->clearQueue();
		return callbackRoomId;
	}

	void draw() {
		display->startWrite();
		{
			display->fillRectangleTx(0, 0, 312, 41, RGB565(0x574b67));
			display->fillRectangleTx(0, 41, 39, 180, RGB565(0x574b67));
			display->fillRectangleTx(0, 221, 312, 19, RGB565(0x574b67));
			display->fillRectangleTx(273, 41, 39, 180, RGB565(0x574b67));
		}
		display->endWrite();
		DrawingUtils::drawTitle(F("Choose a Room"));

		UIHelper::render();
	}

	void setupUI() {
		UIHelper::clearActive();

		exit = new RoundButton(ExitButton);
		exit->setPosition(288, 0);
		exit->callback.bind(&callbackExit);
		UIHelper::registerUI(exit);

		tray = new RoomSelectionGrid();
		tray->setPosition(39, 41);
		tray->callback.bind(&callbackRoomTray);
		UIHelper::registerUI(tray);
	}

	void deallocate() {
		UIHelper::clearActive();
		delete tray;
	}
} // namespace ChooseRoom