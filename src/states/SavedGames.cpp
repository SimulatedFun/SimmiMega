//#include "states/SavedGames.h"
//
//namespace SavedGames {
//	uint8_t numGames;
//	uint8_t selectedGameId;
//	RoundButton* exit;
//	constexpr uint8_t maxTabCount = 4; // todo dynamic in response to number of games on sd
//	NumberTabElement* tabs[maxTabCount];
//	uint8_t currentTab = 0;
//
//	GamePreview* preview[5]; // 5 max per page
//
//	// Popup window
//	GamePreview* mini;
//	SimpleButton *loadBtn, *dupeBtn, *delBtn;
//	RoundButton* exitWindow;
//	Text *loadText, *dupeText, *delText;
//
//	void loop() {
//		UIHelper::loop();
//	}
//
//	/// When changing tabs in the tray
//	void callbackChangeTab(uint8_t number) {
//		if (number == 0 or number > maxTabCount) {
//			ERROR(F("change tab to invalid number!"));
//			return;
//		}
//
//		const uint8_t selectedIndex = number - 1;
//
//		if (currentTab == selectedIndex) {
//			INFO(F("no change to tab"));
//			return;
//		}
//
//		currentTab = selectedIndex;
//		INFO(F("changed tab to ") << number);
//
//		for (uint8_t i = 0; i < maxTabCount; i++) {
//			if (tabs[i] == nullptr) {
//				continue;
//			}
//			if (i == selectedIndex) {
//				tabs[i]->selected = true;
//				tabs[i]->render();
//			} else {
//				if (tabs[i]->selected) {
//					tabs[i]->selected = false;
//					tabs[i]->render();
//				}
//			}
//		}
//		drawGamesList();
//	}
//
//	void drawGamesList() {
//		display->fillRectangle(12, 46, 288, 171, RGB565(0x574b67));
//
//		// de-register all the game preview UI elements
//		for (uint8_t i = 0; i < 5; i++) {
//			UIHelper::deregisterUI(preview[i]);
//		}
//
//		// calculate how many game previews to show on this page
//		const uint8_t gamesLeft = numGames - (currentTab * 5);
//		uint8_t previewsOnPage = gamesLeft;
//		if (gamesLeft > 5) {
//			previewsOnPage = 5;
//		}
//
//		// re-register and set the game id for each game preview ui element
//		for (uint8_t i = 0; i < previewsOnPage; i++) {
//			const uint8_t gameId = (currentTab * 5) + i;
//			preview[i]->gameId = gameId;
//			UIHelper::registerUI(preview[i]);
//			preview[i]->render();
//		}
//	}
//
//	void draw() {
//		INFO(F("draw saved games"));
//		display->fillRectangle(0, 0, 312, 240, RGB565(0x574b67));
//		DrawingUtils::drawTitle(F("Load from SD"));
//
//		display->startWrite();
//		{
//			// bottom two lines
//			display->drawHorizontalLineTx(0, 218, 312, RGB565(0x937ac5));
//			display->drawHorizontalLineTx(0, 217, 312, RGB565(0x2d1b2e));
//		}
//		display->endWrite();
//		UIHelper::render();
//		drawGamesList();
//		LEAK(F("draw() end"));
//	}
//
//	void setup() {
//		// todo count games and set the tab counter and max games vars
//		microSd->begin();
//		const sd_err countCode = microSd->countGameSaves(&numGames);
//		if (countCode != noSdError) {
//			ERROR(F("failed count game saves"));
//		}
//		microSd->end();
//
//		currentTab = 0;
//		allocateTrayUI();
//		registerTrayUI();
//		touch->clearQueue(); // do before draw so you can touch faster
//		draw();
//	}
//
//	void callbackExit(RoundButton&) {
//		deallocateTrayUI();
//		state = MainMenuState;
//	}
//
//	void allocateTrayUI() {
//		exit = new RoundButton(ExitButton);
//		exit->setPosition(288, 0);
//		exit->callback.bind(&callbackExit);
//
//		// allocate the game previews
//		// ui register and game id is set in the draw function
//		for (uint8_t i = 0; i < 5; i++) {
//			preview[i] = new GamePreview(i);
//			preview[i]->setPosition(12, 46 + i * 35);
//			preview[i]->callback.bind(&callbackOpenGame);
//		}
//
//		for (uint8_t i = 0; i < maxTabCount; i++) {
//			tabs[i] = new NumberTabElement(i + 1);
//			tabs[i]->setPosition(i * 24, 219);
//			tabs[i]->callback.bind(&callbackChangeTab);
//		}
//		tabs[0]->selected = true;
//	}
//
//	void registerTrayUI() {
//		UIHelper::registerUI(exit);
//
//		for (uint8_t i = 0; i < maxTabCount; i++) {
//			UIHelper::registerUI(tabs[i]);
//		}
//	}
//
//	void deallocateTrayUI() {
//		LEAK(F("dealloc tray start"));
//		delete exit;
//		// delete the game previews on the page
//		for (uint8_t i = 0; i < 5; i++) {
//			delete preview[i];
//		}
//		for (auto& tab : tabs) {
//			delete tab;
//		}
//		LEAK(F("dealloc tray end"));
//	}
//
//	void allocatePopupUI() {
//		exitWindow = new RoundButton(ExitButton);
//		exitWindow->setPosition(285, 52);
//		exitWindow->callback.bind(&callbackPopupClose);
//
//		mini = new GamePreview(selectedGameId);
//		mini->setPosition(12, 84);
//
//		loadText = new Text(F("Load"));
//		dupeText = new Text(F("Duplicate"));
//		delText = new Text(F("Delete"));
//
//		loadBtn = new SimpleButton();
//		loadBtn->setPosition(12, 123);
//		loadBtn->width = 89;
//		loadBtn->color = purpleBtn;
//		loadBtn->text = loadText;
//		loadBtn->callback.bind(&callbackLoadProject);
//
//		dupeBtn = new SimpleButton();
//		dupeBtn->setPosition(111, 123);
//		dupeBtn->width = 89;
//		dupeBtn->color = purpleBtn;
//		dupeBtn->text = dupeText;
//		dupeBtn->callback.bind(&callbackDuplicateProject);
//
//		delBtn = new SimpleButton();
//		delBtn->setPosition(211, 123);
//		delBtn->width = 89;
//		delBtn->color = redBtn;
//		delBtn->text = delText;
//		delBtn->callback.bind(&callbackDeleteProject);
//	}
//
//	void registerPopupUI() {
//		UIHelper::registerUI(exitWindow);
//		UIHelper::registerUI(loadBtn);
//		UIHelper::registerUI(dupeBtn);
//		UIHelper::registerUI(delBtn);
//	}
//
//	void drawPopupUI() {
//		mini->render();
//		UIHelper::render();
//
//		// draw text in popup window
//		Text* dirText = new Text(F("Directory:"));
//		dirText->setPosition(12, 73);
//		dirText->setStyle(1, BLACK);
//		dirText->print();
//		delete dirText;
//
//		Text* slash = new Text(F("/"));
//		slash->setPosition(64, 73);
//		slash->setStyle(1, 0x000be3);
//		slash->print();
//		delete slash;
//
//		// draw the directory name in dark blue
//
//		// todo consolidate this info and save it for future usage such as when calling loads,
//		// duplicate, delete, etc.
//		Folder folder;
//		Title title;
//		Description desc;
//		microSd->begin();
//		const sd_err code = microSd->loadGameData(selectedGameId, &folder, &title, &desc);
//		microSd->end();
//		if (code != noSdError) {
//			// handle an error when retrieving game information (maybe sd was taken out)
//			microSd->showSdErrorMessage(code);
//			oldState = MainMenuState;
//			state = SavedGamesState;
//			return;
//		}
//
//		Text* directory = new Text(folder.text, directoryMaxLength);
//		directory->setPosition(68, 73);
//		directory->setStyle(1, RGB565(0x000be3)); // dark blue
//		directory->print();
//		delete directory;
//	}
//
//	void deallocatePopupUI() {
//		UIHelper::clearActive();
//		delete mini;
//		delete loadBtn;
//		delete dupeBtn;
//		delete delBtn;
//		delete exitWindow;
//		delete loadText;
//		delete dupeText;
//		delete delText;
//	}
//
//	/// Called when the user selects a game from the game previews list
//	/// Opens the game popup window to [load / duplicate / delete] a project
//	void callbackOpenGame(uint8_t gameId) {
//		selectedGameId = gameId;
//		INFO(F("opened game popup for id: ") << gameId);
//		UIHelper::clearActive();
//		deallocateTrayUI();
//		allocatePopupUI();
//		registerPopupUI();
//		drawPopupWindowBackground();
//		touch.clearQueue();
//		drawPopupUI();
//	}
//
//	void drawPopupWindowBackground() {
//		DrawingUtils::dither(RGB565(0x323232));
//		display->startWrite();
//		{
//			const uint16_t x = 2;
//			const uint16_t y = 51;
//			const uint16_t width = 308;
//			const uint16_t height = 115;
//			// main bg
//			display->fillRectangleTx(x + 4, y + 4, width - 6, 107, RGB565(0xeac592));
//
//			// border
//			display->fillRectangleTx(x + 2, y, width - 4, 2, RGB565(0x2d1b2e));
//			display->fillRectangleTx(x + 2, y + height - 2, width - 4, 2, RGB565(0x2d1b2e));
//			display->fillRectangleTx(x, y + 2, 2, height - 4, RGB565(0x2d1b2e));
//			display->fillRectangleTx(x + width - 2, y + 2, 2, height - 4, RGB565(0x2d1b2e));
//
//			// white inner border
//			display->fillRectangleTx(x + 2, y + 4, 2, height - 12, WHITE);
//			display->fillRectangleTx(x + 4, y + 2, width - 27, 2, WHITE);
//
//			// shadow
//			display->fillRectangleTx(x + 2, y + height - 6, width - 4, 4, RGB565(0xd88b76));
//			display->fillRectangleTx(x + 2, y + height - 8, 2, 2, RGB565(0xd88b76));
//			display->fillRectangleTx(x + width - 4, y + height - 8, 2, 2, RGB565(0xd88b76));
//
//			// corner border
//			display->fillRectangleTx(x + 2, y + 2, 2, 2, RGB565(0x2d1b2e));			 // top left
//			display->fillRectangleTx(x + 2, y + height - 4, 2, 2, RGB565(0x2d1b2e)); // bottom left
//			display->fillRectangleTx(x + width - 4, y + 2, 2, 2, RGB565(0x2d1b2e));	 // top right
//			display->fillRectangleTx(x + width - 4, y + height - 4, 2, 2,
//											 RGB565(0x2d1b2e)); // bottom right
//		}
//		display->endWrite();
//	}
//
//	void callbackPopupClose(RoundButton&) {
//		LEAK(F("popup close start"));
//		exitWindow->depressed = true;
//		exitWindow->render();
//		delay(150);
//		deallocatePopupUI();
//		// hacky heap frag fix
//		oldState = MainMenuState;
//		state = SavedGamesState;
//		touch->clearQueue();
//		LEAK(F("popup close end"));
//	}
//
//	void callbackLoadProject(boolean) {
//		touch->clearQueue();
//		// the game id to load will be selectedGameId
//		// show confirmation warning since it will overwrite all contents in external eeprom
//		// save directory name
//		// get game.dat and save the title and description
//		// get the objects
//		// get the room
//		// get the dialog
//		// get the palettes
//
//
//		deallocatePopupUI();
//
//		Folder folder;
//		Title title;
//		Description desc;
//		microSd->begin();
//		const sd_err code = microSd->loadGameData(selectedGameId, &folder, &title, &desc);
//		microSd->end();
//		if (code != noSdError) {
//			// handle an error when retrieving game information (maybe sd was taken out)
//			microSd->showSdErrorMessage(code);
//			oldState = MainMenuState;
//			state = SavedGamesState;
//			return;
//		}
//
//		// show a confirmation warning before loading/overwriting current project
//		Text* confirm = new Text(F("Confirm"));
//		Text* cancel = new Text(F("Cancel"));
//		Text* main = new Text(F("This will overwrite the\ncurrent project in memory\nwith contents from:\n"));
//		main->setCharactersAtEnd(folder.text, 13);
//		const boolean confirmation = WarningBox::choose(confirm, cancel, main, greenBtn, greyBtn);
//		delete confirm;
//		delete cancel;
//		delete main;
//
//		if (!confirmation) {
//			INFO(F("canceled load project"));
//			// hacky avoids fragmentation
//			oldState = MainMenuState;
//			state = SavedGamesState;
//			return;
//		}
//
//		INFO(F("load project initiated: ") << selectedGameId);
//
//		// overdraw the text area
//		display->fillRectangle(58, 71, 198, 80, RGB565(0xeac592));
//
//		// draw new smaller text area for the save message
//		display->fillRectangle(58, 84, 198, 54, RGB565(0xffe0b6));
//
//		DrawingUtils::simplePrint(66, 101, F("Loading..."), BLACK, 1);
//
//		// progress bar outline
//		display->drawRectangle(66, 109, 183, 21, RGB565(0x2d1b2e));
//
//		const uint16_t yProgress = 110;
//		const uint16_t progressHt = 19;
//		const uint16_t fullWidth = 181;
//		uint16_t xProgress = 67;
//
//		// progress bar inner color
//		display->fillRectangle(xProgress, yProgress, fullWidth, progressHt, RGB565(0xd4ffc8));
//
//		// increment progress bar
//		GameSettings::setDirectory(folder);
//		display->fillRectangle(xProgress++, yProgress, 1, progressHt, RGB565(0x32e020));
//
//		GameSettings::setTitle(title);
//		display->fillRectangle(xProgress++, yProgress, 1, progressHt, RGB565(0x32e020));
//
//		GameSettings::setDescription(desc);
//		display->fillRectangle(xProgress++, yProgress, 1, progressHt, RGB565(0x32e020));
//
//		// todo set starting dialog
//		// todo set starting coords
//		// todo read/write to ext eep all objects, rooms, palettes, etc. and check for existence of stuff and gracefully handle
//		//  basically do as much as we can in case stuff doesn't exist and compile an error list to show at the end
//
//		// fill the whole progress bar
//		display->fillRectangle(67, 110, fullWidth, progressHt, RGB565(0x32e020));
//
//
//	}
//
//	uint8_t stringLength(char* input, uint8_t maxLen) {
//		for (uint8_t i = 0; i < maxLen; i++) {
//			if (input[i] == '\0') {
//				return i;
//			}
//		}
//		return maxLen;
//	}
//
//	/// Takes a directory and modifies it to the next available duplicated variant
//	/// e.g. "myfolder" -> "myfol-01" -> "myfol-02" -> "myfol-15" -> "myfol-16" -> etc.
//	boolean findDirectoryVariant(Folder* folder) {
//		const uint8_t length = stringLength(folder->text, 8);
//
//		for (uint8_t i = 0; i < 99; i++) {
//			uint8_t copyLen = 4;
//			uint8_t startIterator = 4;
//
//			if (length < 4) {
//				copyLen = length;
//				startIterator = length;
//			}
//
//			// copy initial 5 chars to tempdir string
//			char tempDir[8];
//			for (uint8_t j = 0; j < 8; j++) {
//				tempDir[j] = '\0';
//			}
//			strncpy(tempDir, folder->text, copyLen);
//
//			tempDir[startIterator + 0] = '-';
//			tempDir[startIterator + 1] = '0';
//			tempDir[startIterator + 2] = '0';
//
//			// make the number into a string
//			char num[2];
//			itoa(i, num, 10);
//
//			if (i < 10) {
//				strncpy(&tempDir[startIterator + 2], num, 1);
//			} else {
//				strncpy(&tempDir[startIterator + 1], num, 2);
//			}
//
//			if (!SD.exists(tempDir)) {
//				INFO(F("dir not exists, returning"));
//				GameSettings::printSafe(tempDir, 8);
//				Serial << "\n";
//				strncpy(folder->text, tempDir, 8);
//				folder->concat();
//				return true;
//			}
//		}
//		ERROR(F("could not make a variant"));
//		return false;
//	}
//
//	typedef struct {
//			char filename[13];
//	} SdCopy;
//
//	void callbackDuplicateProject(boolean) {
//		deallocatePopupUI();
//
//		WarningBox::drawWindow();
//		RoundButton* exit2 = new RoundButton(ExitButton);
//		exit2->setPosition(249, 32);
//		exit2->render();
//		delete exit2;
//
//		touch.clearQueue();
//
//		// get the original folder name
//		Folder originalFolder;
//		Title title;
//		Description desc;
//		microSd->begin();
//		sd_err code = microSd->loadGameData(selectedGameId, &originalFolder, &title, &desc);
//		microSd->end();
//		if (code != noSdError) {
//			// handle an error when retrieving game information (maybe sd was taken out)
//			microSd->showSdErrorMessage(code);
//			oldState = MainMenuState;
//			state = SavedGamesState;
//			return;
//		}
//
//		// Create the new folder's name based on the current folder
//		// myfolder -> myfold-01 -> myfold-02 -> etc. depending on existing directories
//		Folder newFolder = originalFolder;
//		microSd->begin();
//		const boolean dirSuccess = findDirectoryVariant(&newFolder);
//		microSd->end();
//		if (!dirSuccess) {
//			DrawingUtils::simplePrint(66, 88, F("Error when making a new directory name."), BLACK, 1);
//			ERROR(F("no success making dir"));
//			return;
//		}
//		INFO(F("new folder: ") << newFolder.text);
//
//		// Print out where the files will be moved to
//		DrawingUtils::simplePrint(66, 88 + 14 * 0, F("Copying files to a new directory: "), BLACK,
//											1);
//		// print out new directory
//		Text* msg = new Text();
//		msg->setCharactersAtEnd(newFolder.text, 8);
//		msg->setPosition(66, 88 + 14);
//		msg->setStyle(1, RGB565(0x000be3));
//		msg->print();
//		delete msg;
//
//		// make the new directory to copy files into
//		microSd->begin();
//		if (!microSd->setWorkingDirectoryToRoot()) {
//			ERROR(F("could set dir to root"));
//			return;
//		}
//		// need to make directory
//		code = microSd->makeDirectory(newFolder.text);
//		microSd->end();
//		if (code != noSdError) {
//			ERROR(F("could not make dir"));
//			return;
//		}
//
//		// enter the new directory
//		microSd->begin();
//		microSd->setWorkingDirectoryToRoot();
//		microSd->setWorkingDirectory(newFolder.text);
//		microSd->end();
//
//		// clang-format off
//		SdCopy toCopy[6] = {
//			{"game.dat"},
//			{"dialog.dat"},
//			{"objects.dat"},
//			{"rooms.dat"},
//			{"palettes.dat"},
//			{"splash.bmp"}
//		};
//		// clang-format on
//
//		DrawingUtils::simplePrint(66, 88 + 14 * 3, F("Copying file: "), BLACK, 1);
//
//		constexpr uint16_t bufferSize = 1500;
//		uint8_t buffer[bufferSize];
//		File fileIn;
//		File fileOut;
//
//		// Copy all the relevant root files
//		microSd->begin();
//		for (auto& f : toCopy) {
//
//			uint64_t filePointer = 0; // reset for every file
//
//			display->fillRectangle(130, 88 + 14 * 2, 80, 20, RGB565(0xffe0b6));
//			Text* msg2 = new Text();
//			msg2->setCharactersAtEnd(f.filename, 13);
//			msg2->setPosition(130, 88 + 14 * 3);
//			msg2->setStyle(1, BLACK);
//			msg2->print();
//			delete msg2;
//
//			while (true) {
//				//INFO(F("copying file: ") << f.filename << " ...");
//
//				microSd->setWorkingDirectoryToRoot();
//				microSd->setWorkingDirectory(originalFolder.text);
//				fileIn = SD.open(f.filename, FILE_READ);
//
//				uint16_t bytesRead = 0;
//				fileIn.seekSet(filePointer);
//				if (fileIn.available() > 0) {
//					bytesRead = fileIn.read(buffer, bufferSize);
//				}
//				fileIn.close();
//
//				if (bytesRead == 0) {
//					break;
//				}
//
//				// some bytes were read
//
//				microSd->setWorkingDirectoryToRoot();
//				microSd->setWorkingDirectory(newFolder.text);
//				fileOut = SD.open(f.filename, FILE_WRITE);
//				fileOut.seekSet(filePointer);
//				fileOut.write(buffer, bytesRead);
//				fileOut.close();
//
//				filePointer += bytesRead;
//
//				//INFO(F("copied to file: ") << f.filename << ", bytes: " << bytesRead);
//			}
//		}
//		microSd->end();
//
//		// enter the new directory and make a music directory
//		microSd->begin();
//		microSd->setWorkingDirectoryToRoot();
//		microSd->setWorkingDirectory(newFolder.text);
//		if (!SD.mkdir("music")) {
//			ERROR(F("can't make music dir in new folder"));
//			return;
//		}
//		microSd->end();
//
//		// enter original directory and open the music directory
//		microSd->begin();
//		microSd->setWorkingDirectoryToRoot();
//		microSd->setWorkingDirectory(originalFolder.text);
//		SdFile music;
//		if (!music.open("music")) {
//			ERROR(F("failed to open music directory")); // this is okay, it's not necessary
//			// todo gracefully handle
//			microSd->end();
//			oldState = MainMenuState;
//			state = SavedGamesState;
//			return;
//		}
//
//		// opened music dir, now we want to iterate through all the files and copy them to the new dir
//
//		// todo de-duplicate this code since it's almost the same for the individual files
//		SdFile track;
//		// iterate through all the files in the root directory
//		while (track.openNext(&music, O_RDONLY)) {
//			// ignore hidden files/directories
//			if (track.isHidden() or track.isDirectory()) {
//				microSd->closeSdFile(&track);
//				continue;
//			}
//
//			char filename[13];
//			track.getName(filename, 13);
//
//			display->fillRectangle(130, 88 + 14 * 2, 80, 20, RGB565(0xffe0b6));
//			Text* msg2 = new Text();
//			msg2->setCharactersAtEnd(filename, 13);
//			msg2->setPosition(130, 88 + 14 * 3);
//			msg2->setStyle(1, BLACK);
//			msg2->print();
//			delete msg2;
//
//			const uint16_t yProgress = 135;
//			const uint16_t progressHt = 9;
//			uint16_t xProgress = 67;
//
//			// progress bar outline
//			display->drawRectangle(xProgress - 1, yProgress - 1, 183, progressHt + 2,
//										  RGB565(0x2d1b2e));
//
//			// progress bar inner color
//			display->fillRectangle(xProgress, yProgress, 181, progressHt, RGB565(0xd88b76));
//
//			microSd->setWorkingDirectoryToRoot();
//			microSd->setWorkingDirectory(originalFolder.text);
//			microSd->setWorkingDirectory("music");
//			fileIn = SD.open(filename, FILE_READ);
//
//			const uint64_t fileSize = fileIn.size();
//			const uint32_t numCopyLoops = (fileSize / bufferSize) - 1;
//			const uint16_t increment = (181 / numCopyLoops);
//			const uint16_t everyLoops = (numCopyLoops / 181) + 1;
//
//			//			INFO(F("file size: "));
//			//			print64Bit(fileSize);
//			//			INFO(F("numCopyLoops: ") << numCopyLoops);
//			//			INFO(F("increment: ") << increment);
//			//			INFO(F("everyLoops: ") << everyLoops);
//
//			microSd->setWorkingDirectoryToRoot();
//			microSd->setWorkingDirectory(newFolder.text);
//			microSd->setWorkingDirectory("music");
//			fileOut = SD.open(filename, FILE_WRITE);
//
//			uint16_t currLoop = 0;
//
//			while ((boolean) fileIn.available()) {
//				const uint32_t bytesRead = fileIn.read(buffer, bufferSize);
//				fileOut.write(buffer, bytesRead);
//				if (currLoop % everyLoops == 0) {
//					display->fillRectangle(xProgress++, yProgress, 1, progressHt, RGB565(0xf55023));
//				}
//				currLoop++;
//			}
//
//			fileIn.close();
//			fileOut.close();
//
//			INFO(F("copied file: ") << filename);
//
//			// fill the whole progress bar
//			display->fillRectangle(67, yProgress, 181, 9, RGB565(0xf55023));
//		}
//
//		microSd->end();
//		GOOD(F("done copying files!"));
//
//		oldState = MainMenuState;
//		state = SavedGamesState;
//	}
//
//	void callbackDeleteProject(boolean) {
//		deallocatePopupUI();
//		// game id to delete is selectedGameId
//		// show confirmation warning
//		// delete contents of directory and then the directory itself
//
//		// get the directory of the game we want to delete
//		Folder folder;
//		Title title;
//		Description desc;
//		microSd->begin();
//		const sd_err code = microSd->loadGameData(selectedGameId, &folder, &title, &desc);
//		microSd->end();
//		if (code != noSdError) {
//			// handle an error when retrieving game information (maybe sd was taken out)
//			microSd->showSdErrorMessage(code);
//			oldState = MainMenuState;
//			state = SavedGamesState;
//			return;
//		}
//
//		// show a confirmation warning before deleting the game
//		Text* confirm = new Text(F("Confirm"));
//		Text* cancel = new Text(F("Cancel"));
//		Text* main = new Text(F("This will permanently delete all\ncontents of directory:\n"));
//		main->setCharactersAtEnd(folder.text, 13);
//		const boolean confirmation = WarningBox::choose(confirm, cancel, main, redBtn, greyBtn);
//		delete confirm;
//		delete cancel;
//		delete main;
//
//		if (!confirmation) {
//			INFO(F("canceled delete project"));
//			// hacky avoids fragmentation
//			oldState = MainMenuState;
//			state = SavedGamesState;
//			return;
//		}
//
//		INFO(F("delete project initiated: ") << selectedGameId);
//
//		// overdraw the text area
//		display->fillRectangle(58, 71, 198, 80, RGB565(0xeac592));
//
//		// draw new smaller text area for the save message
//		display->fillRectangle(58, 84, 198, 54, RGB565(0xffe0b6));
//
//		DrawingUtils::simplePrint(66, 101, F("Deleting..."), BLACK, 1);
//
//		// progress bar outline
//		display->drawRectangle(66, 109, 183, 21, RGB565(0x2d1b2e));
//
//		const uint16_t yProgress = 110;
//		const uint16_t progressHt = 19;
//		const uint16_t fullWidth = 181;
//		uint16_t xProgress = 67;
//
//		// progress bar inner color
//		display->fillRectangle(xProgress, yProgress, fullWidth, progressHt, RGB565(0xd4ffc8));
//
//		// remove the entire directory and all contents
//		microSd->begin();
//		const sd_err delCode = microSd->removeWholeDirectory(&folder);
//		microSd->end();
//		if (delCode != noSdError) {
//			ERROR(F("err deleting directory. code: ") << delCode);
//			return;
//		}
//
//		// fill the whole progress bar
//		display->fillRectangle(67, 110, fullWidth, progressHt, RGB565(0x32e020));
//
//		GOOD(F("successful delete!"));
//		oldState = MainMenuState;
//		state = SavedGamesState;
//	}
//} // namespace SavedGames