#include "ui/windows/ChooseRoom.h"

namespace ChooseRoom {
	RoomSelectionGrid* tray = nullptr;

	boolean callbackSelected = false;
	uint16_t callbackRoomId = 0;

	void callbackRoomTray(unsigned int selectedRoomId) {
		callbackSelected = true;
		callbackRoomId = selectedRoomId;
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
		UIHelper::render();
	}

	void setupUI() {
		UIHelper::clearActive();

		tray = new RoomSelectionGrid();
		tray->setPosition(0, 0);
		tray->callback.bind(&callbackRoomTray);
		UIHelper::registerUI(tray);
	}

	void deallocate() {
		UIHelper::clearActive();
		delete tray;
	}
} // namespace ChooseRoom