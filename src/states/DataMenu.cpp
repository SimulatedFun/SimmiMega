#include "states/DataMenu.h"

namespace DataMenu {
	SimpleButton *newGame, *saveGame, *listGames;
	Text *newGameText, *saveGameText, *listGamesText;
	RoundButton* exit;

	void setup() {
		LEAK(F("data menu setup"));
		setupUI();

		draw();
		touch->clearQueue();
	}

	void setupUI() {
		UIHelper::clearActive();

		exit = new RoundButton(ExitButton);
		exit->setPosition(288, 0);
		exit->callback.bind(&callbackExit);
		UIHelper::registerUI(exit);

		newGameText = new Text(F("New Project"));
		saveGameText = new Text(F("Save Project"));
		listGamesText = new Text(F("Games on SD"));

		newGame = new SimpleButton();
		newGame->setPosition(16, 72);
		newGame->text = newGameText;
		newGame->callback.bind(&callbackNewProject);
		UIHelper::registerUI(newGame);

		saveGame = new SimpleButton();
		saveGame->setPosition(112, 72);
		saveGame->text = saveGameText;
		saveGame->callback.bind(&callbackSaveProject);
		UIHelper::registerUI(saveGame);

		listGames = new SimpleButton();
		listGames->setPosition(208, 72);
		listGames->text = listGamesText;
		listGames->callback.bind(&callbackListGames);
		UIHelper::registerUI(listGames);
	}

	void deallocate() {
		UIHelper::clearActive();
		delete exit;
		delete newGameText;
		delete saveGameText;
		delete listGamesText;
		delete newGame;
		delete saveGame;
		delete listGames;
	}

	void loop() {
		UIHelper::loop();
	}

	void draw() {
		DrawingUtils::fill(RGB565(0x574b67));
		DrawingUtils::drawTitle(F("Data Management"));

		UIHelper::render();

		drawTextBox(16, 104, 88, 93);
		drawTextBox(112, 104, 88, 78);
		drawTextBox(208, 104, 88, 66);

		Text* newProjDesc = new Text("Wipes device\nmemory and\nstarts a new\nproject. Does\nnot affect SD\ncard data.");
		newProjDesc->setPosition(23, 120);
		newProjDesc->setStyle(1, RGB565(0x5b4a68));
		newProjDesc->print();
		delete newProjDesc;

		Text* saveProjectDesc = new Text("Saves to SD.\nOverwrites the\ncontents of\ndirectory:");
		saveProjectDesc->setPosition(119, 120);
		saveProjectDesc->setStyle(1, RGB565(0x5b4a68));
		saveProjectDesc->print();
		delete saveProjectDesc;

		Folder folder;
		GameSettings::getDirectory(&folder);

		Text* directory = new Text(folder.text, directoryMaxLength);
		directory->setPosition(119, 175);
		directory->setStyle(1, RGB565(0x000be3));  // dark blue
		directory->print();
		delete directory;

		Text* gamesList = new Text("Load a game\ninto onboard\nmemory from\nthe SD card.");
		gamesList->setPosition(215, 120);
		gamesList->setStyle(1, RGB565(0x5b4a68));
		gamesList->print();
		delete gamesList;
	}

	void drawTextBox(uint8_t x, uint8_t y, uint8_t width, uint8_t height) {
		display->startWrite();
		{
			display->fillRectangleTx(x, y, width, height, RGB565(0xeac592));

			// top left
			display->drawPixelTx(x, y, RGB565(0x5b4a68));
			display->drawPixelTx(x + 1, y, RGB565(0x5b4a68));
			display->drawPixelTx(x, y + 1, RGB565(0x5b4a68));

			// top right
			display->drawPixelTx(x + width - 1, y, RGB565(0x5b4a68));
			display->drawPixelTx(x + width - 1, y + 1, RGB565(0x5b4a68));
			display->drawPixelTx(x + width - 2, y, RGB565(0x5b4a68));

			// bottom left
			display->drawPixelTx(x, y + height - 1, RGB565(0x5b4a68));
			display->drawPixelTx(x + 1, y + height - 1, RGB565(0x5b4a68));
			display->drawPixelTx(x, y + height - 2, RGB565(0x5b4a68));

			// bottom right
			display->drawPixelTx(x + width - 1, y + height - 1, RGB565(0x5b4a68));
			display->drawPixelTx(x + width - 2, y + height - 1, RGB565(0x5b4a68));
			display->drawPixelTx(x + width - 1, y + height - 2, RGB565(0x5b4a68));
		}
		display->endWrite();
	}

	void callbackExit(RoundButton&) {
		deallocate();
		state = MainMenuState;
	}

	void callbackNewProject(boolean) {
		deallocate();

		LEAK(F("callbackNewProject dealloc ready for warning box"));

		Text* confirm = new Text(F("Confirm"));
		Text* cancel = new Text(F("Cancel"));
		Text* main = new Text("This will clear all unsaved changes\nmade to the project.\n\nContinue?");
		const boolean confirmation = WarningBox::choose(confirm, cancel, main, redBtn, greyBtn);
		delete confirm;
		delete cancel;
		delete main;

		if (!confirmation) {
			INFO(F("canceled new project operation"));
			// hacky avoids fragmentation
			oldState = MainMenuState;
			state = DataManagementState;
			return;
		}

		// overdraw the text area
		display->fillRectangle(58, 71, 198, 80, RGB565(0xeac592));

		// draw new smaller text area for the delete message
		display->fillRectangle(58, 84, 198, 54, RGB565(0xffe0b6));

		DrawingUtils::simplePrint(66, 101, "Deleting...", BLACK, 1);

		// progress bar outline
		display->drawRectangle(66, 109, 183, 21, RGB565(0x2d1b2e));

		uint16_t yProgress = 110;
		uint16_t progressHt = 19;
		uint16_t xProgress = 67;

		// progress bar inner color
		display->fillRectangle(xProgress, yProgress, 181, progressHt, RGB565(0xd88b76));

		uint32_t address = 0;
		uint16_t offset = 10;
		for (uint32_t i = 0; i < EEPROM_BYTE_LEN; i += 1000) {
			eeprom->clear(address, address + 1000);
			display->fillRectangle(xProgress, yProgress, offset, progressHt, RGB565(0xf55023));
			address += 1000;
			offset += 4;
		}

		touch->writeEepromCalibration(); // then rewrite calibration to memory

		display->fillRectangle(xProgress, yProgress, 140, progressHt, RGB565(0xf55023));

		ram->clear();

		display->fillRectangle(xProgress, yProgress, 150, progressHt, RGB565(0xf55023));

		GOOD(F("reset game objects and rooms"));

		// make palettes a default gradient instead of all black
		Palette pal(WHITE, DARK_GREY, BLACK);
		for (uint8_t i = 0; i < paletteCount; i++) {
			pal.id = i;
			pal.save();
		}
		display->fillRectangle(xProgress, yProgress, 160, progressHt, RGB565(0xf55023));
		GOOD(F("reset palettes"));

		// clear dialog
		for (uint8_t i = 0; i < dialogCount; i++) {
			Dialog dialog(i);
			dialog.clear();
			dialog.save();
		}
		display->fillRectangle(xProgress, yProgress, 170, progressHt, RGB565(0xf55023));
		GOOD(F("cleared dialogs"));

		Title title;
		strlcpy(title.text.text, "Default Title", titleMaxLength);
		GameSettings::setTitle(title);

		Description desc;
		strlcpy(desc.text, "Default Description", descriptionMaxLength);
		GameSettings::setDescription(desc);

		Folder folder;
		strcpy(folder.text, "default");
		GameSettings::setDirectory(folder);

		const Coordinates coords{6, 5, 0}; // center-ish of room 0
		GameSettings::setStartingCoords(coords);

		// what dialog shows when you first start the game
		GameSettings::setStartingDialog(0);

		const char msg[] = {"Default dialog that shows at the beginning of the game\0"};
		Dialog* dialog = new Dialog(0);
		for (uint16_t i = 0; i < sizeof(msg); i++) {
			dialog->setCharacterAtIndex(i, Text::asciiToGlyphLookupId(msg[i]));
		}
		dialog->save();
		delete dialog;

		// fill the whole progress bar
		display->fillRectangle(67, 110, 181, 19, RGB565(0xf55023));

		// draw new smaller text area for the delete message
		display->fillRectangle(58, 84, 198, 54, RGB565(0xffe0b6));

		DrawingUtils::simplePrint(66, 101, "Done!", BLACK, 1);

		// progress bar outline
		display->drawRectangle(66, 109, 183, 21, RGB565(0x2d1b2e));

		yProgress = 110;
		progressHt = 19;
		xProgress = 67;

		// progress bar inner color
		display->fillRectangle(xProgress, yProgress, 181, progressHt, RGB565(0x21d253));

		delay(750);

		// todo reset room thumbnails

		// hacky avoids fragmentation
		oldState = MainMenuState;
		state = DataManagementState;
	}

	void callbackSaveProject(boolean) {
		deallocate();
		LEAK(F("callbackSaveProject dealloc ready for warning box"));

		Text* confirm = new Text(F("Confirm"));
		Text* cancel = new Text(F("Cancel"));
		char text[85] = "Confirm you want to save to SD.\n\nThis will overwrite the directory:\n\"";

		Folder folder;
		GameSettings::getDirectory(&folder);
		uint8_t cursor = 69;
		const uint8_t folderLen = GameSettings::getStringLength(folder.text, directoryMaxLength);
		strlcpy(&text[cursor], folder.text, folderLen + 1); // appends a null term automatically
		cursor += folderLen;
		text[cursor++] = '\"';
		text[cursor] = '\0';

		Text* main = new Text(text, 85);
		const boolean confirmation = WarningBox::choose(confirm, cancel, main, greenBtn, greyBtn);
		delete confirm;
		delete cancel;
		delete main;

		if (!confirmation) {
			INFO(F("canceled save project"));
			// hacky avoids fragmentation
			oldState = MainMenuState;
			state = DataManagementState;
			return;
		}

		// overdraw the text area
		display->fillRectangle(58, 71, 198, 80, RGB565(0xeac592));

		// draw new smaller text area for the save message
		display->fillRectangle(58, 84, 198, 54, RGB565(0xffe0b6));

		DrawingUtils::simplePrint(66, 101, F("Saving..."), BLACK, 1);

		// progress bar outline
		display->drawRectangle(66, 109, 183, 21, RGB565(0x2d1b2e));

		const uint16_t yProgress = 110;
		const uint16_t progressHt = 19;
		const uint16_t fullWidth = 181;
		uint16_t xProgress = 67;

		// progress bar inner color
		display->fillRectangle(xProgress, yProgress, fullWidth, progressHt, RGB565(0xd4ffc8));

		microSd->begin();

		uint8_t errCode = noSdError;

		errCode = setOrMakeGameDirectory();
		if (checkIfError(errCode)) {
			return;
		}
		display->fillRectangle(xProgress, yProgress, 25, progressHt, RGB565(0x32e020));
		xProgress += 25;

		errCode = saveSettingsToFile();
		if (checkIfError(errCode)) {
			return;
		}
		display->fillRectangle(xProgress, yProgress, 25, progressHt, RGB565(0x32e020));
		xProgress += 25;

		errCode = saveGameObjectsToFile();
		if (checkIfError(errCode)) {
			return;
		}
		display->fillRectangle(xProgress, yProgress, 25, progressHt, RGB565(0x32e020));
		xProgress += 25;

		errCode = saveRoomsToFile();
		if (checkIfError(errCode)) {
			return;
		}
		display->fillRectangle(xProgress, yProgress, 25, progressHt, RGB565(0x32e020));
		xProgress += 25;

		errCode = savePalettesToFile();
		if (checkIfError(errCode)) {
			return;
		}
		display->fillRectangle(xProgress, yProgress, 25, progressHt, RGB565(0x32e020));
		xProgress += 25;

		errCode = saveDialogToFile();
		if (checkIfError(errCode)) {
			return;
		}

		microSd->end();

		// fill the whole progress bar
		display->fillRectangle(67, 110, 181, 19, RGB565(0x32e020));

		// hacky avoids fragmentation
		oldState = MainMenuState;
		state = DataManagementState;
	}

	void callbackListGames(boolean) {
		deallocate();
		state = SavedGamesState;
	}

	boolean checkIfError(uint8_t errCode) {
		if (errCode == noSdError) {
			return false;
		}

		ERROR(F("save settings failed"));
		display->fillRectangle(44, 56, 2266, 145, RGB565(0xeac592));
		DrawingUtils::simplePrint(66, 88, "Error saving game settings to file.\n\nError code:", BLACK, 1);

		Text* text = new Text(errCode);
		text->setStyle(2, BLACK);
		text->setPosition(66, 158);
		text->print();
		delete text;

		// todo wait until touch?
		while (!touch->isPressed()) {
			touch->poll();
		}

		oldState = MainMenuState;
		state = DataManagementState;
		return true;
	}

	sd_err setOrMakeGameDirectory() {
		if (!microSd->sdTransactionOpen) {
			return errSdTransactionNotOpen;
		}

		// todo
//		if (!SD.chdir()) {
//			return errNoRoot;
//		}

		Folder folder;
		GameSettings::getDirectory(&folder);
		INFO(F("directory: ") << folder.text);

		// create the directory
		if (!SD.exists(folder.text)) {
			ERROR(F("dir does not exist, making dir: ") << folder.text);
			if (!SD.mkdir(folder.text)) {
				ERROR(F("failed to create dir: ") << folder.text);
				return errFailMakeDir;
			}
		}

		// todo	// open into directory
//		if (!Storage::setWorkingDirectory(folder.text)) {
//			ERROR(F("failed to open dir: ") << folder.text);
//			return errFailOpenDir;
//		}

		return noSdError;
	}

	sd_err saveSettingsToFile() {
		if (!microSd->sdTransactionOpen) {
			return errSdTransactionNotOpen;
		}

		// write to file, clearing if necessary
		File file = SD.open("game.dat", FILE_WRITE);
		// todo truncate file
		if (!file) {
			ERROR(F("failed to open game.dat"));
			return errFailOpenFile;
		}

		Title title;
		GameSettings::getTitle(&title);
		file.print(title.text.text);
		file.print("\n");

		Description desc;
		GameSettings::getDescription(&desc);
		file.print(desc.text);
		file.print("\n");

		uint8_t startingDialogId = 0;
		GameSettings::getStartingDialog(&startingDialogId);
		file.print(startingDialogId);
		file.print("\n");

		Coordinates c(0, 0, 0);
		GameSettings::getStartingCoords(&c);
		file.print(c.x);
		file.print(", ");
		file.print(c.y);
		file.print("\n");

		file.close();
		return noSdError;
	}

	uint8_t saveGameObjectsToFile() {
		if (!microSd->sdTransactionOpen) {
			return errSdTransactionNotOpen;
		}

		// write to file, clearing if necessary
		File file = SD.open("objects.dat", FILE_WRITE);
		// todo truncate file

		if (!file) {
			ERROR(F("failed to open objects.dat"));
			return errFailOpenFile;
		}

		for (uint16_t i = 0; i < objectCount; i++) {
			GameObject obj(i);
			obj.load();

			file.print("// GameObject ");
			file.print(i);
			file.print("\n");

			//microSd->write64Bit(&file, obj.data.frame1);
			file.println();
			//microSd->write64Bit(&file, obj.data.frame2);
			file.println();
			for (uint8_t j = 0; j < logicDataStructSize; j++) {
				file.println(obj.data.logic[j]);
			}
			file.println();
		}

		file.close();
		return noSdError;
	}

	uint8_t saveRoomsToFile() {
		if (!microSd->sdTransactionOpen) {
			return errSdTransactionNotOpen;
		}

		// write to file, clearing if necessary
		File file = SD.open("rooms.dat", FILE_WRITE);

		if (!file) {
			ERROR(F("failed to open rooms.dat"));
			return errFailOpenFile;
		}

		for (uint8_t roomId = 0; roomId < roomCount; roomId++) {
			file.print("// Room ");
			file.print(roomId);
			file.print("\n");

			uint16_t gid = 0;
			for (uint8_t y = 0; y < 10; y++) {
				for (uint8_t x = 0; x < 13; x++) {
					const Coordinates coords(x, y, roomId);
					RoomHelper::getGameObjectId(&gid, coords);
					file.print(gid);
					if (x != 12) {
						file.print(", ");
					}
				}
				file.print("\n");
			}

			uint8_t trackId = _NO_MUSIC;
			RoomHelper::getMusicId(&trackId, roomId);
			file.print(trackId);
			file.print("\n");

			uint8_t paletteId = _NO_PALETTE;
			RoomHelper::getPaletteId(&paletteId, roomId);
			file.print(paletteId);
			file.print("\n");

			uint16_t objectId = _NO_GAMEOBJECT;
			RoomHelper::getPlayerGameObjectId(&objectId, roomId);
			file.print(objectId);
			file.print("\n\n");
		}

		file.close();
		return noSdError;
	}

	uint8_t savePalettesToFile() {
		if (!microSd->sdTransactionOpen) {
			return errSdTransactionNotOpen;
		}

		// write to file, clearing if necessary
		File file = SD.open("palettes.dat", FILE_WRITE);
		if (!file) {
			ERROR(F("failed to open palettes.dat"));
			return errFailOpenFile;
		}

		Palette* pal = new Palette();
		for (pal->id = 0; pal->id < paletteCount; pal->id++) {
			pal->load();

			file.print("// Palette ");
			file.print(pal->id);
			file.print("\n");

			file.println(pal->getHighlight());
			file.println(pal->getForeground());
			file.println(pal->getBackground());

			file.print("\n");
		}
		delete pal;

		file.close();
		return noSdError;
	}

	uint8_t saveDialogToFile() {
		if (!microSd->sdTransactionOpen) {
			return errSdTransactionNotOpen;
		}

		// write to file, clearing if necessary
		File file = SD.open("dialog.dat", FILE_WRITE);

		if (!file) {
			ERROR(F("failed to open dialog.dat"));
			return errFailOpenFile;
		}

		for (uint16_t i = 0; i < dialogCount; i++) {
			Dialog dialog(i);
			dialog.load();

			file.print("// Dialog ");
			file.print(i);
			file.print("\n");

			for (uint16_t j = 0; j < dialogMaxLength; j++) {
				file.print(dialog.buffer[j]);
				if (j != dialogMaxLength - 1) {
					file.print(", ");
				}
			}
			file.print("\n");
			file.print("\n");
		}

		file.close();
		return noSdError;
	}
} // namespace DataMenu