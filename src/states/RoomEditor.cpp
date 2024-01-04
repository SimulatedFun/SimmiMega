#include "states/RoomEditor.h"

// todo add border to obj select and room edit so you know where you can press
// todo add music selection per room
// todo

namespace RoomEditor {
	uint8_t editRoomId = _NO_ROOM;
	boolean selectedTool = pencilTool;

	RoundButton *exit, *pencil, *eraser, *clear;
	RoomEditGrid* editGrid;
	RoomTraySelectGrid* tray;
	ObjectSelector* player;
	PaletteSelector* paletteSelector;
	MusicSelector* music;

	constexpr uint8_t maxTabCount = 5;
	NumberTabElement* tabs[maxTabCount];

	Palette* palette;
	GameObject* playerObject;

	void loadData() {
		palette = new Palette();
		INFO(F("get palette for room editor"));
		RoomHelper::getPaletteId(&palette->id, editRoomId);
		palette->load();

		uint16_t objId = _NO_GAMEOBJECT;
		RoomHelper::getPlayerGameObjectId(&objId, editRoomId);
		playerObject = new GameObject(objId);
		playerObject->load();
	}

	void setup() {
		editRoomId = ChooseRoom::pick();
		if (editRoomId == _NO_ROOM) {
			state = MainMenuState;
			return;
		}

		loadData();
		setupUI();

		selectedTool = pencilTool;
		resetTimer(MarchingAntsAnimation);
		touch->clearQueue();

		draw();
	}

	void setupUI() {
		UIHelper::clearActive();

		exit = new RoundButton(ExitButton);
		exit->setPosition(288, 0);
		exit->callback.bind(&callbackExit);
		UIHelper::registerUI(exit);

		pencil = new RoundButton(PencilButton);
		pencil->setPosition(220, 136);
		pencil->depressed = true;
		pencil->callback.bind(&callbackChoosePencil);
		UIHelper::registerUI(pencil);

		eraser = new RoundButton(EraserButton);
		eraser->setPosition(244, 136);
		eraser->depressed = false;
		eraser->callback.bind(&callbackChooseEraser);
		UIHelper::registerUI(eraser);

		clear = new RoundButton(ClearButton);
		clear->setPosition(268, 136);
		clear->callback.bind(&callbackClear);
		UIHelper::registerUI(clear);

		player = new ObjectSelector();
		player->setPosition(224, 108);
		player->setData(playerObject, palette);
		player->callback.bind(&callbackChoosePlayer);
		UIHelper::registerUI(player);

		paletteSelector = new PaletteSelector();
		paletteSelector->setPosition(224, 76);
		paletteSelector->setData(palette);
		paletteSelector->callback.bind(&callbackChoosePalette);
		UIHelper::registerUI(paletteSelector);

		music = new MusicSelector();
		music->setPosition(224, 44);
		RoomHelper::getMusicId(&music->trackId, editRoomId);
		music->callback.bind(&callbackMusic);
		UIHelper::registerUI(music);

		editGrid = new RoomEditGrid();
		editGrid->roomId = editRoomId;
		editGrid->setPosition(4, 4);
		editGrid->palette = palette;
		editGrid->callback.bind(&callbackPlaceObject);
		UIHelper::registerUI(editGrid);

		tray = new RoomTraySelectGrid();
		tray->setPosition(4, 169);
		tray->palette = palette;
		tray->highlightedGameObjectId = 0;
		tray->currentTab = 0;
		tray->callback.bind(&callbackSelectTray);
		UIHelper::registerUI(tray);

		for (uint8_t i = 0; i < maxTabCount; i++) {
			tabs[i] = new NumberTabElement(i + 1);
			tabs[i]->setPosition(i * 24, 219);
			tabs[i]->callback.bind(&callbackChangeTab);
			UIHelper::registerUI(tabs[i]);
		}
		tabs[0]->selected = true;

		// music 224, 44
	}

	void callbackChoosePalette(boolean) {
		deallocate();

		boolean cancelled = false;
		uint16_t paletteId = 0;
		ChoosePalette::pick(true, &paletteId, &cancelled);

		if (!cancelled) {
			RoomHelper::setPaletteId(paletteId, editRoomId);
		}

		loadData();
		setupUI();
		draw();
	}

	/// When changing tabs in the tray
	void callbackChangeTab(uint8_t number) {
		if (number == 0 or number > maxTabCount) {
			ERROR(F("change tab to invalid number!"));
			return;
		}

		const uint8_t selectedIndex = number - 1;

		if (tray->currentTab == selectedIndex) {
			INFO(F("no change to tab"));
			return;
		}

		tray->currentTab = selectedIndex;

		// sets to the first obj in tray
		tray->highlightedGameObjectId = (tray->currentTab * objectsPerTray);
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

	void callbackChoosePlayer(boolean) {
		deallocate();

        boolean cancelled = false;
        uint16_t gameObjectId = 0;
        ChooseObject::pick(false, &gameObjectId, &cancelled);
        if (!cancelled) {
            RoomHelper::setPlayerGameObjectId(gameObjectId, editRoomId);
        }

		loadData();
		setupUI();
		draw();
	}

	void loop() {
		UIHelper::loop();
		loopMarchingAnts();
	}

	/// Draws an animated dashed border around the selected object
	void loopMarchingAnts() {
		if (!checkTimer(150, MarchingAntsAnimation, true)) {
			return;
		}

		const uint8_t indexInTray =
				  tray->highlightedGameObjectId - (tray->currentTab * objectsPerTray);
		const uint16_t x = 4 + ((indexInTray % objectsPerTrayRow) * 16);
		const uint16_t y = 169 + ((indexInTray / objectsPerTrayRow) * 16);

		static uint8_t offset = 0;
		display->startWrite();
		{
			// traverse width
			for (uint16_t i = 0; i < MARCHING_ANTS_WIDTH; i++) {
				if ((i + offset) % 5 >= 3) { // every 2 pixels
					display->drawPixelTx(x + i, y, WHITE);
					display->drawPixelTx(x + i, y + MARCHING_ANTS_HEIGHT, WHITE);
				} else {
					display->drawPixelTx(x + i, y, BLACK);
					display->drawPixelTx(x + i, y + MARCHING_ANTS_HEIGHT, BLACK);
				}
			}

			for (uint16_t i = 0; i < MARCHING_ANTS_HEIGHT; i++) {
				if ((i + offset) % 5 >= 3) {
					display->drawPixelTx(x, y + i, WHITE);
					display->drawPixelTx(x + MARCHING_ANTS_WIDTH, y + i, WHITE);
				} else {
					display->drawPixelTx(x, y + i, BLACK);
					display->drawPixelTx(x + MARCHING_ANTS_WIDTH, y + i, BLACK);
				}
			}
		}
		display->endWrite();
		offset++; // todo eventual overflow
	}

	void draw() {
		DrawingUtils::fill(RGB565(0x574b67));
		DrawingUtils::simplePrint(248, 120, F("player"), RGB565(0x9d7bc9), 1);
		DrawingUtils::simplePrint(248, 88, F("palette"), RGB565(0x9d7bc9), 1);
		DrawingUtils::simplePrint(221, 37, F("music"), RGB565(0x9d7bc9), 1);
		UIHelper::render();
	}

	void deallocate() {
		LEAK(F("room edit dealloc start"));
		UIHelper::clearActive();
		delete exit;
		delete pencil;
		delete eraser;
		delete clear;
		delete editGrid;
		delete tray;
		delete player;
		delete paletteSelector;
		for (auto& tab : tabs) {
			delete tab;
		}

		delete palette;
		delete playerObject;
		LEAK(F("room edit dealloc end"));
	}

	// region Callbacks
	void callbackExit(RoundButton&) {
		// no saving necessary since the place object callback saves to ram and eeprom
		oldState = MainMenuState;
        state = RoomEditorState;
		FileManager::saveRoomThumbnailData(editRoomId);
		deallocate();
	}

	void callbackChoosePencil(RoundButton&) {
		if (selectedTool == pencilTool) {
			return;
		}
		selectedTool = pencilTool;
		pencil->depressed = true;
		eraser->depressed = false;
		pencil->render();
		eraser->render();
	}

	void callbackChooseEraser(RoundButton&) {
		if (selectedTool == eraserTool) {
			return;
		}
		selectedTool = eraserTool;
		pencil->depressed = false;
		eraser->depressed = true;
		pencil->render();
		eraser->render();
	}

	void callbackClear(RoundButton&) {
		clear->depressed = true;
		clear->render();
		INFO(F("callbackClear"));
		for (uint8_t x = 0; x < 13; x++) {
			for (uint8_t y = 0; y < 10; y++) {
				RoomHelper::setGameObjectId(_NO_GAMEOBJECT, Coordinates(x, y, editRoomId));
				const uint8_t index = y * 13 + x;
				editGrid->drawObjectInRoom(index);
			}
		}
		// no delay needed since the clear operation is slow
		clear->depressed = false;
		clear->render();
	}

	void callbackPlaceObject(uint8_t index) {
		const uint16_t x = index % 13;
		const uint16_t y = index / 13;
		const Coordinates coords(x, y, editRoomId);
		uint8_t newGameObjectId = tray->highlightedGameObjectId; // pencil mode
		if (selectedTool == eraserTool) {
			newGameObjectId = _NO_GAMEOBJECT;
		}

		uint16_t oldGameObjectId;
		RoomHelper::getGameObjectId(&oldGameObjectId, coords);

		// gameobject didn't change, so don't do anything
		if (newGameObjectId == oldGameObjectId) {
			DEBUG(F("no change to object at location"));
			return;
		}

		INFO(F("callbackPlaceObject at: (")
			  << coords.x << ", " << coords.y << ") id: " << newGameObjectId);

		// save the new object and draw it in room
		RoomHelper::setGameObjectId(newGameObjectId, coords);
		editGrid->drawObjectInRoom(index);
	}

	void callbackSelectTray(boolean) {
		if (selectedTool != pencilTool) {
			selectedTool = pencilTool;
			pencil->depressed = true;
			pencil->render();
			eraser->depressed = false;
			eraser->render();
		}
	}

	void callbackMusic(boolean) {
		deallocate();

		const uint8_t newTrackId = ChooseMusic::pick();
		RoomHelper::setMusicId(newTrackId, editRoomId);

		loadData();
		setupUI();
		draw();
	}
	// endregion
} // namespace RoomEditor
