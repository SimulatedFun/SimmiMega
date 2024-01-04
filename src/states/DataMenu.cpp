#include "states/DataMenu.h"

namespace DataMenu {
	SimpleButton *newGame, *saveGame, *listGames;
	Text *newGameText, *saveGameText, *listGamesText;
	RoundButton* exit;

	void setup() {
		LEAK("data menu setup");
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

		Text* newProject;
		newProject = new Text("Wipes device\nmemory and\nstarts a new\nproject. "
									 "Does\nnot affect SD\ncard data.");
		newProject->setPosition(23, 120);
		newProject->setStyle(1, RGB565(0x5b4a68));
		newProject->print();
		delete newProject;

		Text* saveProjectDesc = new Text("Saves to SD.\nOverwrites the\ncontents of\ndirectory:");
		saveProjectDesc->setPosition(119, 120);
		saveProjectDesc->setStyle(1, RGB565(0x5b4a68));
		saveProjectDesc->print();
		delete saveProjectDesc;

		// print current project's directory in this same text box
		Folder folder;
		GameSettings::getDirectory(&folder);
		Text* directory = new Text(folder.text, directoryMaxLength);
		directory->setPosition(119, 175);
		directory->setStyle(1, RGB565(0x000be3)); // blue
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
		Text* main = new Text("This will clear all unsaved changes\nmade to the project. "
									 "Consider saving\nto SD first.\n\nContinue anyways?");
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

		WarningBox::setupProgressBar(36, RGB565(0xd88b76));
		DrawingUtils::simplePrint(66, 101, "Deleting...", BLACK, 1);

		uint32_t address = 0;
		uint16_t offset = 10;
		for (uint32_t i = 0; i < EEPROM_BYTE_LEN; i += 1000) {
			eeprom->clear(address, address + 1000);
			WarningBox::stepProgressBar(RGB565(0xf55023));
			address += 1000;
			offset += 4;
		}

		touch->writeEepromCalibration(); // then rewrite calibration to memory

		WarningBox::stepProgressBar(RGB565(0xf55023));

		ram->clear();

		WarningBox::stepProgressBar(RGB565(0xf55023));

		GOOD(F("reset game objects and rooms"));

		// make palettes a default gradient instead of all black
		Palette pal(WHITE, DARK_GREY, BLACK);
		for (uint8_t i = 0; i < paletteCount; i++) {
			pal.id = i;
			pal.save();
		}
		WarningBox::stepProgressBar(RGB565(0xf55023));
		GOOD(F("reset palettes"));

		// clear dialog
		for (uint8_t i = 0; i < dialogCount; i++) {
			Dialog dialog(i);
			dialog.clear();
			dialog.save();
		}
		WarningBox::stepProgressBar(RGB565(0xf55023));
		GOOD(F("cleared dialogs"));

		Title title;
		strlcpy(title.text, "Default Title", titleMaxLength);
		GameSettings::setTitle(title);

		Description desc;
		strlcpy(desc.text, "Default Description", descriptionMaxLength);
		GameSettings::setDescription(desc);

		Folder folder;
		strcpy(folder.text, "default");
		GameSettings::setDirectory(folder);

		WarningBox::stepProgressBar(RGB565(0xf55023));

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

		WarningBox::finishProgressBar(RGB565(0x21d253));

		DrawingUtils::simplePrint(66, 101, "Done!", BLACK, 1);

		delay(750);

		// hacky avoids fragmentation
		oldState = MainMenuState;
		state = DataManagementState;
	}

	void callbackSaveProject(boolean) {
		deallocate();
		LEAK(F("callbackSaveProject dealloc ready for warning box"));

		Text* confirm = new Text(F("Confirm"));
		Text* cancel = new Text(F("Cancel"));
		char text[85] = "Confirm you want to save to SD.\n\nThis will overwrite the directory:\n\"/";

		// add the directory name to the popup text
		Folder folder;
		GameSettings::getDirectory(&folder);

		uint8_t cursor = 70;
		const uint8_t folderLen = GameSettings::getStringLength(folder.text, directoryMaxLength);
		strncpy(&text[cursor], folder.text, folderLen + 1); // appends a null term automatically
		cursor += folderLen;
		text[cursor++] = '\"';
		text[cursor] = '\0';

		Text* main = new Text(text, 85);
		const boolean confirmation = WarningBox::choose(confirm, cancel, main, greenBtn, greyBtn);
		delete confirm;
		delete cancel;
		delete main;

		if (!confirmation) {
			INFO("canceled save project");
			// hacky avoids fragmentation
			oldState = MainMenuState;
			state = DataManagementState;
			return;
		}

		WarningBox::setupProgressBar(181, RGB565(0xd4ffc8));
		DrawingUtils::simplePrint(66, 101, F("Saving Project..."), BLACK, 1);

		const String directory = String(folder.text, folderLen);

		// make sure the directory exists first, don't check for errors since it might already exist
		microSd->begin();
		microSd->createDirTx(directory);
		microSd->end();
		WarningBox::stepProgressBar();

		sd_err errCode = saveSettingsToFile(directory);
		if (checkIfError(errCode, "Error saving settings file.")) {
			return;
		}
		Serial.println("saved settings to file");
		WarningBox::stepProgressBar();

		errCode = saveGameObjectsToFile(directory);
		if (checkIfError(errCode, "Error saving gameobjects.")) {
			return;
		}
		Serial.println("saved game objects to file");
		WarningBox::stepProgressBar();

		errCode = saveRoomsToFile(directory);
		if (checkIfError(errCode, "Error saving rooms.")) {
			return;
		}
		WarningBox::stepProgressBar();

		errCode = savePalettesToFile(directory);
		if (checkIfError(errCode, "Error saving palettes.")) {
			return;
		}
		WarningBox::stepProgressBar();

		errCode = saveDialogToFile(directory);
		if (checkIfError(errCode, "Error saving dialog.")) {
			return;
		}

		WarningBox::finishProgressBar();
		DrawingUtils::simplePrint(66, 101, "Done!", BLACK, 1);
		delay(750);

		// hacky avoids fragmentation
		oldState = MainMenuState;
		state = DataManagementState;
	}

	void callbackListGames(boolean) {
		deallocate();
		state = SavedGamesState;
	}

	boolean checkIfError(uint8_t errCode, str message) {
		if (errCode == noSdError) {
			return false;
		}

		ERROR(F("save settings failed"));
		display->fillRectangle(44, 56, 2266, 145, RGB565(0xeac592));
		DrawingUtils::simplePrint(66, 88, message, BLACK, 1);

		Text* text = new Text(errCode);
		text->setStyle(2, BLACK);
		text->setPosition(66, 193);
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

	sd_err saveSettingsToFile(const String& directory) {
		const String filePath = "/" + directory + "/game.dat";
		Serial.printf("attempting to save: %s\n", filePath.c_str());

		microSd->deleteFile(filePath);
		WarningBox::stepProgressBar();

		// formulate the contents of the file in a simple buffer, then open the file and save it
		char buffer[150];
		uint8_t cursor = 0;

		// helper function to append to buffer
		auto appendToBuffer = [&](const char* text) {
			cursor += strlcpy(&buffer[cursor], text, sizeof(buffer) - cursor);
		};

		appendToBuffer("// Game metadata\n");
		WarningBox::stepProgressBar();

		Title title;
		GameSettings::getTitle(&title);
		appendToBuffer(title.text);
		appendToBuffer("\n");
		WarningBox::stepProgressBar();

		Description desc;
		GameSettings::getDescription(&desc);
		appendToBuffer(desc.text);
		appendToBuffer("\n");
		WarningBox::stepProgressBar();

		uint8_t startingDialogId = 0;
		GameSettings::getStartingDialog(&startingDialogId);
		char startDialogStr[4];
		sprintf(startDialogStr, "%d\n", startingDialogId);
		appendToBuffer(startDialogStr);
		WarningBox::stepProgressBar();

		// Append starting coordinates
		Coordinates c(0, 0, 0);
		GameSettings::getStartingCoords(&c);
		char coordStr[10];
		sprintf(coordStr, "%d, %d, %d", c.x, c.y, c.roomId);
		appendToBuffer(coordStr);
		WarningBox::stepProgressBar();

		// now open the file, and write the contents
		microSd->begin();
		{
			File file = SD.open(filePath, FILE_WRITE);
			if (!file) {
				Serial.printf("failed to open %s", filePath.c_str());
				microSd->end();
				return errFailOpenFile;
			}

			file.print(buffer);
			file.close();
		}
		microSd->end();
		WarningBox::stepProgressBar();

		return noSdError;
	}

	uint8_t saveGameObjectsToFile(const String& directory) {
		// create the file path string
		const String filePath = "/" + directory + "/objects.dat";
		Serial.printf("attempting to save: %s\n", filePath.c_str());

		microSd->deleteFile(filePath);
		WarningBox::stepProgressBar();

		char buffer[100]; // minimum is 44 (20, 20, and 4 chars to represent a full game object)
		uint8_t cursor = 0;

		// helper function to append to buffer
		auto appendToBuffer = [&](const char* text) {
			cursor += strlcpy(&buffer[cursor], text, sizeof(buffer) - cursor);
		};

        // top of file information about the format
        appendToBuffer("// File stores gameobject data:\n"
                       "// - two 64-bit sprite lines (8x8 bit array)\n"
                       "// - an 11-bit (expandable) logic number for flags like \"is animated,\" \"is solid,\" etc.\n"
                       "// Flag details in manual.");
        microSd->begin();
        {
            // Write to file
            File file = SD.open(filePath, FILE_APPEND);
            if (!file) {
                Serial.printf("failed to open %s", filePath.c_str());
                microSd->end();
                return errFailOpenFile;
            }

            file.print(buffer);
            file.close();
        }
        microSd->end();
        WarningBox::stepProgressBar();

		for (uint16_t objectId = 0; objectId < objectCount; objectId++) {
			// clear the buffer and reset cursor
			for (uint16_t i = 0; i < 100; i++) {
				buffer[i] = 0;
			}
			cursor = 0;

			GameObject obj(objectId);
			obj.load();

			for (uint16_t i = 0; i < 100; i++) {
				buffer[i] = 0;
			}

			char notation[21];
			sprintf(notation, "// GameObject %d\n", objectId); // Biggest string is "// GameObject 16384\n\0"
			appendToBuffer(notation);

			char frame1[22];
			sprintf(frame1, "%llu\n", obj.data.frame1); // Biggest string is "18446744073709551616\n\0"
			appendToBuffer(frame1);

			char frame2[22];
			sprintf(frame2, "%llu\n", obj.data.frame2);
			appendToBuffer(frame2);

			for (uint16_t i = 0; i < logicDataStructSize - 1; i++) {
				char logic[6];
				sprintf(logic, "%d, ", obj.data.logic[i]); // Biggest string is "255, \0"
				appendToBuffer(logic);
			}
			char logic[8];
			sprintf(logic, "%d\n\n", obj.data.logic[logicDataStructSize - 1]);
			appendToBuffer(logic);

			microSd->begin();
			{
				File file = SD.open(filePath, FILE_APPEND);
				if (!file) {
					Serial.printf("failed to open %s", filePath.c_str());
					microSd->end();
					return errFailOpenFile;
				}

				file.print(buffer);
				file.close();
			}
			microSd->end();

			WarningBox::stepProgressBar();
		}

		return noSdError;
	}

	sd_err saveRoomsToFile(const String& directory) {
		// create the file path string
		const String filePath = "/" + directory + "/rooms.dat";
		Serial.printf("attempting to save: %s\n", filePath.c_str());

		microSd->deleteFile(filePath);
		WarningBox::stepProgressBar();

		char buffer[1100];
		uint16_t cursor = 0;

		// helper function to append to buffer
		auto appendToBuffer = [&](const char* text) {
			cursor += strlcpy(&buffer[cursor], text, sizeof(buffer) - cursor);
		};

        // top of file information about the format
        appendToBuffer("// File contains data for 36 rooms (0-35), each with a 10x13 array of gameobject IDs.\n"
                       "// IDs are 16-bit, stored as 10-bit in EEPROM.\n"
                       "// Below each array are player ID, paletteID, and BGM track ID.    ");
        microSd->begin();
        {
            // Write to file
            File file = SD.open(filePath, FILE_APPEND);
            if (!file) {
                Serial.printf("failed to open %s", filePath.c_str());
                microSd->end();
                return errFailOpenFile;
            }

            file.print(buffer);
            file.close();
        }
        microSd->end();
        WarningBox::stepProgressBar();

		// writing code to handle game object IDs up to 16384
		for (uint8_t roomId = 0; roomId < roomCount; roomId++) {
			// a room is 13x10 so a line might look like: 16384, 16384, ...
			// 7 chars * 12 + 5 + newline = 90
			// min is 90 per line, but just in case we'll do 100

			// clear the buffer and reset cursor
			for (uint16_t i = 0; i < 1100; i++) {
				buffer[i] = 0;
			}
			cursor = 0;

			char notation[12];
			sprintf(notation, "// Room %d\n", roomId); // Biggest string is "// Room 36\n\0"
			appendToBuffer(notation);


			uint16_t gid = 0;
			Coordinates coordinates;
			coordinates.roomId = roomId;
			char num[7];
			for (uint8_t y = 0; y < 10; y++) {
				for (uint8_t x = 0; x < 13; x++) {
					coordinates.x = x;
					coordinates.y = y;
					RoomHelper::getGameObjectId(&gid, coordinates);
					if (x == 12) {
						sprintf(num, "%d\n", gid);
					} else {
						sprintf(num, "%d, ", gid);
					}
					appendToBuffer(num);
				}
			}

			uint16_t playerId = _NO_GAMEOBJECT;
			RoomHelper::getPlayerGameObjectId(&playerId, roomId);

			uint8_t paletteId = _NO_PALETTE;
			RoomHelper::getPaletteId(&paletteId, roomId);

			uint8_t musicId = _NO_MUSIC;
			RoomHelper::getMusicId(&musicId, roomId);

			char extra[18];
			sprintf(extra, "%hu, %d, %d\n\n", playerId, paletteId, musicId); // Biggest string is "16384, 128, 128\n\n\0"
			appendToBuffer(extra);

			microSd->begin();
			{
				// Write to file
				File file = SD.open(filePath, FILE_APPEND);
				if (!file) {
					Serial.printf("failed to open %s", filePath.c_str());
					microSd->end();
					return errFailOpenFile;
				}

				file.print(buffer);
				file.close();
			}
			microSd->end();

			WarningBox::stepProgressBar();
		}
		return noSdError;
	}

	uint8_t savePalettesToFile(const String& directory) {
		// create the file path string
		const String filePath = "/" + directory + "/palettes.dat";
		Serial.printf("attempting to save: %s\n", filePath.c_str());

		// remove the palettes.dat file if it exists
		microSd->deleteFile(filePath);
		WarningBox::stepProgressBar();

		for (uint8_t paletteId = 0; paletteId < paletteCount; paletteId++) {
			Palette p;
			p.id = paletteId;
			p.load();

			char buffer[38];
			sprintf(buffer, "// Palette %d\n%d, %d, %d\n\n", paletteId, p.getHighlight(),
					  p.getForeground(),
					  p.getBackground()); // Biggest string is "// Room 36\n\0"

			microSd->begin();
			{
				// Write to file
				File file = SD.open(filePath, FILE_APPEND);
				if (!file) {
					Serial.printf("failed to open %s", filePath.c_str());
					microSd->end();
					return errFailOpenFile;
				}

				file.print(buffer);
				file.close();
			}
			microSd->end();

			WarningBox::stepProgressBar();
		}

		return noSdError;
	}

	uint8_t saveDialogToFile(const String& directory) {
		// create the file path string
		const String filePath = "/" + directory + "/dialog.dat";
		Serial.printf("attempting to save: %s\n", filePath.c_str());

		// remove the dialog.dat file if it exists
		microSd->begin();
		{
			if (SD.remove(filePath)) {
				Serial.println("removed dialog.dat file");
			} else {
				Serial.println("no dialog.dat file to remove");
			}
		}
		microSd->end();
		WarningBox::stepProgressBar();

		char buffer[dialogMaxLength + 2 * dialogMaxLength + 20];
		uint16_t cursor = 0;

		// helper function to append to buffer
		auto appendToBuffer = [&](const char* text) {
			cursor += strlcpy(&buffer[cursor], text, sizeof(buffer) - cursor);
		};

		for (uint16_t dialogId = 0; dialogId < dialogCount; dialogId++) {
			//Serial.printf("starting to store dialog %d in buffer\n", dialogId);
			// clear the buffer and reset cursor
			for (uint16_t i = 0; i < sizeof(buffer); i++) {
				buffer[i] = 0;
			}
			cursor = 0;

			Dialog dialog(dialogId);
			dialog.load();

			char header[17]; // "// Dialog 16384n0"
			sprintf(header, "// Dialog %hu\n", dialogId);
			appendToBuffer(header);

			// print dialog chars to a csv-like file
			for (uint16_t j = 0; j < dialogMaxLength; j++) {
				const uint8_t c = dialog.buffer[j];

				char letter[6];														 // "128, \0"
				if (j == dialogMaxLength - 1 or c == GLYPH_END_OF_LINE) { // last char for this dialog
					sprintf(letter, "%u\n\n", c);
				} else {
					sprintf(letter, "%u, ", c);
				}
				appendToBuffer(letter);
				if (c == GLYPH_END_OF_LINE) {
					break;
				}
			}

			//Serial.printf("buffer: %s\n", buffer);

			microSd->begin();
			{
				// Write to file
				File file = SD.open(filePath, FILE_APPEND);
				if (!file) {
					Serial.printf("failed to open %s", filePath.c_str());
					microSd->end();
					return errFailOpenFile;
				}

				file.print(buffer);
				file.close();
			}
			microSd->end();

			WarningBox::stepProgressBar();
		}

		return noSdError;
	}
} // namespace DataMenu