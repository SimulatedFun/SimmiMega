#include "ui/windows/ChooseDialog.h"
#include "ui/selection-grid/Dialog.h"

namespace ChooseDialog {
	DialogSelectionGrid* tray;
	RoundButton* exit;
	constexpr uint8_t maxTabCount = 2;
	NumberTabElement* tabs[maxTabCount];
	uint8_t currentTab = 0;
	boolean showZeroth = true;

	boolean callbackSelected = false;
	boolean callbackCancelled = false;
	uint16_t callbackDialogId = 0;

	void callbackDialogTray(unsigned int row) {
		callbackDialogId = row + (currentTab * dialogsPerTab);
		if (!tray->showZeroth) { // offset if we're not zero indexed
			callbackDialogId++;
		}
		if (callbackDialogId >= dialogCount) { // if trying to select dialog out of bounds
			callbackDialogId = 0;
			callbackSelected = false;
			return;
		}
		callbackSelected = true;
	}

	void callbackExit(RoundButton&) {
		callbackCancelled = true;
	}

	/// When changing tabs in the tray
	void callbackChangeTab(uint8_t tabNumber) {
		if (tabNumber == 0 or tabNumber > maxTabCount) {
			ERROR(F("change tab to invalid tabNumber!"));
			return;
		}

		const uint8_t newTabNumber = tabNumber - 1;

		if (currentTab == newTabNumber) {
			INFO(F("no change to tab"));
			return;
		}

		currentTab = newTabNumber;
		INFO(F("changed tab to ") << tabNumber);

        // press the newly selected tab, un-press any of the other tabs
		for (uint8_t i = 0; i < maxTabCount; i++) {
			if (tabs[i] == nullptr) {
				continue;
			}
			if (i == newTabNumber) {
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
		callbackCancelled = false;
		callbackDialogId = 0;
	}

	void pick(boolean inShowZerothDialog, uint16_t* dialogId, boolean* cancelled) {
		showZeroth = inShowZerothDialog;
		setup();

		while (!callbackSelected and !callbackCancelled) {
			UIHelper::loop();
		}

		if (callbackCancelled) {
			*cancelled = true;
            INFO("closed choose dialog window");
		}

		deallocate();
		touch->clearQueue();
        INFO(F("ChooseDialog::pick() returns ") << callbackDialogId);

		*dialogId = callbackDialogId;
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
