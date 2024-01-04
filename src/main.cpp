#include "main.h"

// todo edit title, description in settings menu
// todo fix start position choose in settings menu
// todo show icon/bmp in settings
// todo add calibration function in settings menu and secret way on boot
// todo show games on sd
// todo save game to sd

// Global variables initialized from "states/States.h"
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

void setup() {
	Serial.begin(115200);
	Serial.printf("\n"
					  " _____ _             _ _____\n"
					  "|   __|_|_____ _____|_|     |___ ___ ___\n"
					  "|__   | |     |     | | | | | -_| . | .'|\n"
					  "|_____|_|_|_|_|_|_|_|_|_|_|_|___|_  |__,|\n"
					  "                                |___|\n");

	bus = new SPIClass(HSPI);
	bus->begin(SHARED_CLK, SHARED_MISO, SHARED_MOSI, -1);

	display = new Display(bus);
	display->initialize();

	DrawingUtils::drawSplash();

	touch = new Touch(bus);
	touch->initialize();

	eeprom = new ExtEeprom(bus);
	if (!eeprom->initialize()) {
		Serial.println("eeprom could not init");
		Text* msg = new Text(F("External eeprom did not respond.\n\n1. Check for metal debris on pins\n2. Power off, then back on."));
		Text* btn = new Text(F("Okay"));
		WarningBox::showMessage(msg, btn, greyBtn);
		delete msg;
		delete btn;
		while(true);
	}

	ram = new RamBlock();
	ram->initialize();

	Serial.printf("Init complete\n");

	// touch->calibrate();
	// touch->readEepromCalibration();
	touch->loadHardcodedCalibration();
	syncEepromAndRam();

	microSd = new MicroSD(bus);
	// todo handle sd card not being present since it's not necessary for game engine operation
	microSd->initialize();

	// ############################# Set Palette 0 to a black/white gradient
//	Palette* pal = new Palette();
//	pal->id = 0;
//	pal->load(false);
//	pal->setColorByIndex(PALETTE_HIGHLIGHT, WHITE);
//	pal->setColorByIndex(PALETTE_FOREGROUND, DARK_GREY);
//	pal->setColorByIndex(PALETTE_BACKGROUND, BLACK);
//	pal->save();
//	delete pal;
	// #############################

//	microSd->begin();
//	const uint8_t count = FileManager::countGameSavesTx();
//	Serial.printf("games found on SD card: %d\n", count);
//	microSd->end();
}

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

/// Loads entire game from external eeprom into ram block
void syncEepromAndRam() {
	uint64_t buffer = 0;
	for (uint32_t i = 0; i < RAM_EEPROM_BYTE_COUNT / 8; i++) {
		eeprom->read(&buffer, i * 8, 8);
		ram->write(buffer, i * 8, 8);
	}
	GOOD(F("synced eeprom and ram"));
}