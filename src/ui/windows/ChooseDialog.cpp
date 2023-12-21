#include "ui/windows/ChooseDialog.h"
#include "ui/selection-grid/Dialog.h"

namespace ChooseDialog {
	DialogSelectionGrid* tray;
	RoundButton* exit;
	constexpr uint8_t maxTabCount = 4;
	NumberTabElement* tabs[maxTabCount];
	uint8_t currentTab = 0;

	boolean callbackSelected = false;
	uint16_t callbackDialogId = _NO_DIALOG;

	void callbackDialogTray(unsigned int selectedDialogId) {
		callbackSelected = true;
		callbackDialogId = selectedDialogId + (currentTab * dialogsPerTab);
	}

	void callbackExit(RoundButton&) {
		callbackSelected = true;
		callbackDialogId = _NO_DIALOG;
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
			} else {
				if (tabs[i]->selected) {
					tabs[i]->selected = false;
					tabs[i]->render();
				}
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

		tray = new DialogSelectionGrid();
		tray->setPosition(0, 48);
		tray->callback.bind(&callbackDialogTray);
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
		display->fillRectangle(0, 0, 312, 240, RGB565(0x574b67));
		DrawingUtils::drawTitle(F("Choose Dialog"));

		display->startWrite();
		{
			// bottom two lines
			display->drawHorizontalLineTx(0, 218, 312, RGB565(0x937ac5));
			display->drawHorizontalLineTx(0, 217, 312, RGB565(0x2d1b2e));
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
		INFO(F("ChooseDialog::pick() returns ") << callbackDialogId);
		UIHelper::clearActive();
		touch->clearQueue();
		return callbackDialogId;
	}

	void deallocate() {
		UIHelper::clearActive();
		for (auto& tab : tabs) {
			delete tab;
		}
		delete tray;
		delete exit;
	}
} // namespace ChooseDialog
