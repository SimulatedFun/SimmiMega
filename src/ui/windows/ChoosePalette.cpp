#include "ui/windows/ChoosePalette.h"

namespace ChoosePalette {
	PaletteSelectionGrid* tray;
	RoundButton* exit;
	constexpr uint8_t maxTabCount = 2;
	NumberTabElement* tabs[maxTabCount];
	uint8_t currentTab = 0;

	boolean callbackSelected = false;
	uint8_t callbackPaletteId = _NO_PALETTE;

	void setup() {
		currentTab = 0;
		setupUI();
		touch->clearQueue(); // do before draw so you can touch faster
		draw();
		callbackSelected = false;
	}

	void setupUI() {
		UIHelper::clearActive();

		exit = new RoundButton(ExitButton);
		exit->setPosition(288, 0);
		exit->callback.bind(&callbackExit);
		UIHelper::registerUI(exit);

		tray = new PaletteSelectionGrid();
		tray->setPosition(0, 36);
		tray->callback.bind(&callbackPaletteTray);
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
		display->fillRectangle(0, 0, 312, 217, RGB565(0x574b67));
		DrawingUtils::drawTitle(F("Choose a Palette"));

		display->startWrite();
		{
			// separator above the tabs
			display->drawHorizontalLineTx(0, 218, 312, RGB565(0x937ac5));
			display->drawHorizontalLineTx(0, 217, 312, RGB565(0x2d1b2e));

			// bottom purple
			display->fillRectangleTx(0, 219, 312, 21, RGB565(0x574b67));
		}
		display->endWrite();

		UIHelper::render();
	}

	void callbackPaletteTray(uint8_t selectedPaletteId) {
		callbackSelected = true;
		callbackPaletteId = selectedPaletteId + (currentTab * palettesPerTab);
	}

	void callbackExit(RoundButton&) {
		callbackSelected = true;
		callbackPaletteId = _NO_PALETTE;
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

	uint16_t pick() {
		setup();

		while (!callbackSelected) {
			UIHelper::loop();
		}

		deallocate();
		INFO(F("ChoosePalette::pick() returns ") << callbackPaletteId);
		touch->clearQueue();
		return callbackPaletteId;
	}

	void deallocate() {
		UIHelper::clearActive();
		for (auto& tab : tabs) {
			delete tab;
		}
		delete tray;
		delete exit;
	}
}