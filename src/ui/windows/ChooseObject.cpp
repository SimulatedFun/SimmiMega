#include "ui/windows/ChooseObject.h"

namespace ChooseObject {
	ObjectSelectionGrid* tray;
	RoundButton* exit;
	constexpr uint8_t maxTabCount = 4;
	NumberTabElement* tabs[maxTabCount];
	uint8_t currentTab = 0;

	boolean callbackSelected = false;
	uint16_t callbackGameObjectId = 0;

	void callbackObjectTray(unsigned int selectedObjId) {
		callbackSelected = true;
		callbackGameObjectId = selectedObjId + (currentTab * objectsPerTab);
	}

	void callbackExit(RoundButton&) {
		callbackSelected = true;
		callbackGameObjectId = _NO_GAMEOBJECT;
	}

	/// When changing tabs in the tray
	void callbackChangeTab(uint8_t number) {
		if (number == 0 or number > maxTabCount) {
			ERROR(F("change tab to invalid number!"));
			return;
		}

		const uint8_t selectedIndex = number - 1;

		if (currentTab == selectedIndex) {
			INFO(F("no change to tab"));
			return;
		}

		currentTab = selectedIndex;
		INFO(F("changed tab to ") << number);

		for (uint8_t i = 0; i < maxTabCount; i++) {
			if (tabs[i] == nullptr) {
				continue;
			}
			if (i == selectedIndex) {
				tabs[i]->selected = true;
				tabs[i]->render();
			} else if (tabs[i]->selected) {
				tabs[i]->selected = false;
				tabs[i]->render();
			}
		}
		tray->render();
	}

	void setupUI() {
		UIHelper::clearActive();

		exit = new RoundButton(ExitButton);
		exit->setPosition(288, 0);
		exit->callback.bind(&callbackExit);
		UIHelper::registerUI(exit);

		tray = new ObjectSelectionGrid();
		tray->setPosition(0, 35);
		tray->callback.bind(&callbackObjectTray);
		tray->setPageRef(&currentTab);
		UIHelper::registerUI(tray);

		for (uint8_t i = 0; i < maxTabCount; i++) {
			tabs[i] = new NumberTabElement(i + 1);
			tabs[i]->setPosition(i * 24, 219);
			tabs[i]->callback.bind(&callbackChangeTab);
			UIHelper::registerUI(tabs[i]);
		}
		tabs[0]->selected = true;
	}

	void draw() {
		display->fillRectangle(0, 0, 312, 36, RGB565(0x574b67));

		DrawingUtils::drawTitle(F("Choose an Object"));

		display->startWrite();
		{
			// precise fill
			display->drawVerticalLineTx(0, 36, 180, RGB565(0x574b67));
			display->drawVerticalLineTx(311, 36, 180, RGB565(0x574b67));

			for (uint8_t i = 0; i < 11; i++) {
				display->fillRectangleTx(25 + (i * 26), 36, 2, 180, RGB565(0x574b67));
			}

			for (uint8_t i = 0; i < 6; i++) {
				display->fillRectangleTx(1, 60 + (i * 26), 310, 2, RGB565(0x574b67));
			}

			// bottom two lines
			display->drawHorizontalLineTx(0, 216, 312, RGB565(0x574b67));
			display->drawHorizontalLineTx(0, 217, 312, RGB565(0x2d1b2e));
			display->drawHorizontalLineTx(0, 218, 312, RGB565(0x937ac5));

			// purple behind the tabs
			display->fillRectangleTx(0, 219, 312, 21, RGB565(0x574b67));
		}
		display->endWrite();

		UIHelper::render();
	}

	void setup() {
		currentTab = 0;
		setupUI();
		touch->clearQueue(); // do before draw so you can touch faster
		draw();
		callbackSelected = false;
	}

	uint16_t pick() {
		setup();

		while (!callbackSelected) {
			UIHelper::loop();
		}

		deallocate();
		INFO(F("ChooseObject::pick() returns ") << callbackGameObjectId);
		UIHelper::clearActive();
		touch->clearQueue();
		return callbackGameObjectId;
	}

	void deallocate() {
		UIHelper::clearActive();
		for (auto& tab : tabs) {
			delete tab;
		}
		delete tray;
		delete exit;
	}
} // namespace ChooseObject