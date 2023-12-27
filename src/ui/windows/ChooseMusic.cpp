#include "ui/windows/ChooseMusic.h"

namespace ChooseMusic {
	MusicSelectionGrid* tray;
	RoundButton* exit;
	constexpr uint8_t maxTabCount = 5;
	NumberTabElement* tabs[maxTabCount];
	uint8_t currentTab = 0;

	boolean callbackSelected = false;
	uint8_t callbackTrackId = _NO_MUSIC;

	void setup() {
		currentTab = 0;
		setupUI();
		touch->clearQueue(); // do before draw so you can touch faster
		draw();
		callbackSelected = false;
		callbackTrackId = _NO_MUSIC;
	}

	void setupUI() {
		UIHelper::clearActive();

		exit = new RoundButton(ExitButton);
		exit->setPosition(288, 0);
		exit->callback.bind(&callbackExit);
		UIHelper::registerUI(exit);

		tray = new MusicSelectionGrid();
		tray->setPosition(0, 48);
		tray->callback.bind(&callbackMusicTray);
		tray->setPageRef(&currentTab);
		UIHelper::registerUI(tray);

		tray->play = new RoundButton(PlayButton);
		tray->play->callback.bind(&callbackPlay);
		tray->stop = new RoundButton(StopButton);
		tray->stop->callback.bind(&callbackStop);

		for (uint8_t i = 0; i < maxTabCount; i++) {
			tabs[i] = new NumberTabElement(i + 1);
			tabs[i]->setPosition(i * 24, 219);
			tabs[i]->callback.bind(&callbackChangeTab);
			UIHelper::registerUI(tabs[i]);
		}
		tabs[0]->selected = true;
		LEAK(F("choose music setupUI done"));
	}

	void draw() {
		display->fillRectangle(0, 0, 312, 217, RGB565(0x574b67));

		DrawingUtils::drawTitle(F("Choose a Track"));

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

	void callbackMusicTray(uint8_t selectedTrackId) {
		callbackSelected = true;
		callbackTrackId = selectedTrackId;
	}

	void callbackExit(RoundButton&) {
		callbackSelected = true;
		callbackTrackId = _NO_MUSIC;
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

	void callbackPlay(RoundButton&) {
		Audio::stop();
		tray->play->depressed = true;
		tray->play->render();

		String filename;

		microSd->begin();
		FileManager::getSongInfoTx(tray->playingTrackId, &filename);
		microSd->end();

		INFO(F("playing song: ") << filename);

		Folder gameFolder;
		GameSettings::getDirectory(&gameFolder);

		INFO(F("file path: ") << filename);
		Audio::playFile(String(gameFolder.text) + "/" + filename);
	}

	void callbackStop(RoundButton&) {
		tray->stop->depressed = true;
		tray->stop->render();
		Audio::stop();
		delay(200);
		tray->stop->depressed = false;
		tray->stop->render();
	}

	uint8_t pick() {
		LEAK(F("chosoe music pick()"));
		setup();

		while (!callbackSelected) {
			UIHelper::loop();
			Audio::buffer();
		}

		deallocate();
		INFO(F("ChooseMusic::pick() returns ") << callbackTrackId);
		touch->clearQueue();
		Audio::stop();
		return callbackTrackId;
	}

	void deallocate() {
		UIHelper::clearActive();
		for (auto& tab : tabs) {
			delete tab;
		}
		delete tray;
		delete exit;
	}
} // namespace ChooseMusic