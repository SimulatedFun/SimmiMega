#include "ui/windows/ChoosePalette.h"

namespace ChoosePalette {
	PaletteSelectionGrid* tray;
	RoundButton* exit;
	constexpr uint8_t maxTabCount = 2;
	NumberTabElement* tabs[maxTabCount];
	uint8_t currentTab = 0;
	boolean showZeroth = true;

	boolean callbackSelected = false;
	boolean callbackCancelled = false;
	uint8_t callbackPaletteId = 0;

	void setup() {
		currentTab = 0;
		setupUI();
		touch->clearQueue(); // do before draw so you can touch faster
		draw();
		callbackSelected = false;
		callbackCancelled = false;
		callbackPaletteId = 0;
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
		tray->showZeroth = showZeroth;
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
		if (!tray->showZeroth) {
			callbackPaletteId++;
		}
	}

	void callbackExit(RoundButton&) {
		callbackCancelled = true;
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

	void pick(boolean includeZerothPalette, uint16_t* paletteId, boolean* cancelled) {
		showZeroth = includeZerothPalette;
		setup();

		while (!callbackSelected and !callbackCancelled) {
			UIHelper::loop();
		}

		if (callbackCancelled) {
			*cancelled = true;
            INFO("closed choose palette window");
		}

		deallocate();
		touch->clearQueue();
        INFO(F("ChoosePalette::pick() returns ") << callbackPaletteId);

		*paletteId = callbackPaletteId;
	}

	void deallocate() {
		UIHelper::clearActive();
		for (auto& tab : tabs) {
			delete tab;
		}
		delete tray;
		delete exit;
	}
} // namespace ChoosePalette