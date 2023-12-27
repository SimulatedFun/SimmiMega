#include "main.h"
//   _____ _             _ _____
//  |   __|_|_____ _____|_|     |___ ___ ___
//  |__   | |     |     | | | | | -_| . | .'|
//  |_____|_|_|_|_|_|_|_|_|_|_|_|___|_  |__,|
//                                  |___|

// todo all editors should not exit to main menu - should go back to choose window

// Global variables initialized from "states/states.h"
State state = MainMenuState; // Starting state upon initialization
State oldState = OffState;	  // Old state - what it was before

// Global objects
SPIClass* bus;
Touch* touch;
Display* display;
ExtEeprom* eeprom;
RamBlock* ram;
MicroSD* microSd;
GFXfont* currentFont;

// Debugging variables
boolean debugEeprom = false;
boolean debugRam = false;
boolean debugPathfinding = false;
boolean profileTargetPathfinding = false;

// TODO use ram block for playing game, use eeprom for saving data

void setup() {
	Serial.begin(115200);
	Serial.printf("Hello!\n");

	bus = new SPIClass(HSPI);
	bus->begin(SHARED_CLK, SHARED_MISO, SHARED_MOSI, -1);

	display = new Display(bus);
	display->initialize();

	display->startWrite();
	{
		display->fillRectangleTx(312, 0, 8, 240, BLACK);
		display->fillRectangleTx(0, 0, 312, 240, RGB565(0x574b67));
		display->fillRectangleTx(40, 40, 232, 160, RGB565(0x6c5b83));
	}
	display->endWrite();
	drawRGBBitmap(58, 91, intro, 197, 43, 0xf81f);
	DrawingUtils::simplePrint(87, 150, "Micro Game-Making Machine", WHITE, 1);
	DrawingUtils::simplePrint(93, 225, "Made by Adam Kehl 2023", RGB565(0x909090), 1);

	eeprom = new ExtEeprom(bus);
	eeprom->initialize();

	ram = new RamBlock();
	ram->initialize();

	touch = new Touch(bus);
	touch->initialize();

	Serial.printf("Init complete\n");

	//touch->calibrate();
	//touch->readEepromCalibration();
	touch->loadHardcodedCalibration();
	//touch->drawMatrix();
	syncEepromAndRam();

	microSd = new MicroSD(bus);
	microSd->initialize();

	// ############################# Set Palette 0 to a black/white gradient
	Palette* pal = new Palette();
	pal->id = 0;
	pal->load(false);
	pal->setColorByIndex(PALETTE_HIGHLIGHT, WHITE);
	pal->setColorByIndex(PALETTE_FOREGROUND, DARK_GREY);
	pal->setColorByIndex(PALETTE_BACKGROUND, BLACK);
	pal->save();
	delete pal;
	// #############################

	// ############################# Set start coordinates and start dialog
	//	Coordinates start{0, 0, 0};
	//	GameSettings::setStartingCoords(start);
	//	GameSettings::getStartingCoords(&start);
	//
	//	uint8_t dialogId = 0;
	//	GameSettings::setStartingDialog(dialogId);
	//	GameSettings::getStartingDialog(&dialogId);
	// #############################

	// #############################
	// Reset all 130 positions in the room to game object 0
	//	for (uint8_t x = 0; x < 13; x++) {
	//		for (uint8_t y = 0; y < 10; y++) {
	//			const Coordinates c = Coordinates(x, y, 0);
	//			RoomHelper::setGameObjectId(0, c);
	//		}
	//	}
	//
	//	RoomHelper::setPlayerGameObjectId(0, 0);
	//	GameSettings::setStartingCoords(Coordinates(4, 4, 0));
	// #############################

	microSd->begin();
	const uint8_t count = FileManager::countGameSavesTx();
	Serial.printf("count: %d\n", count);
	microSd->end();
}

// void test() {
//	microSd->test();
//
//	eeprom->test();
//
//	Glyphs::debugFont();
//
//	{
//		DrawingUtils::testRefreshRate();
//		DrawingUtils::fillCheckerboard(RED, WHITE);
//		DrawingUtils::dither(DARK_GREY, false);
//		DrawingUtils::fill(BLACK);
//	} // Test screen refresh rate
// }

void loop() {
	if (state != PlayState) {
		touch->poll();
	}

	Commands::check();

	// If the state changes, it runs the setup function, else it loops inside each namespace
	static boolean stateChange;
	stateChange = (state != oldState);
	oldState = state;

	switch (state) {
		case PlayState:
			stateChange ? Play::setup() : Play::loop();
			break;
		case MainMenuState:
			stateChange ? MainMenu::setup() : MainMenu::loop();
			break;
		case SpriteEditorState:
			stateChange ? SpriteEditor::setup() : SpriteEditor::loop();
			break;
		case RoomEditorState:
			stateChange ? RoomEditor::setup() : RoomEditor::loop();
			break;
		case LogicEditorState:
			stateChange ? LogicEditor::setup() : LogicEditor::loop();
			break;
		case DialogEditorState:
			stateChange ? DialogEditor::setup() : DialogEditor::loop();
			break;
		case PaletteEditorState:
			stateChange ? PaletteEditor::setup() : PaletteEditor::loop();
			break;
		case SettingsEditorState:
			stateChange ? SettingsMenu::setup() : SettingsMenu::loop();
			break;
		case DataManagementState:
			stateChange ? DataMenu::setup() : DataMenu::loop();
			break;
			//		case SavedGamesState:
			//			stateChange ? SavedGames::setup() : SavedGames::loop();
			//			break;
		default:
			state = MainMenuState;
			oldState = OffState; // forces rerun of state change to main menu
			break;
	}
}

/// Loads entire game from external eeprom into ram
void syncEepromAndRam() {
	uint64_t buffer; // transfer buffer
	uint64_t check = 0;
	for (uint32_t i = 0; i < RAM_EEPROM_BYTE_COUNT / sizeof(buffer); i++) {
		const uint16_t address = i * sizeof(buffer);
		eeprom->read(&buffer, address, sizeof(buffer));
		ram->write(buffer, address, sizeof(buffer));
		ram->read(&check, address, sizeof(buffer));
		if (buffer != check) {
			ERROR(F("doesn't match: "));
			print64Bit(buffer);
			RAW(F(" -> "));
			print64Bit(check);
			RAW(endl);
		}
	}
	GOOD(F("loaded eeprom into ram"));
}