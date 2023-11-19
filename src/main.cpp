#include "main.h"
//   _____ _             _ _____
//  |   __|_|_____ _____|_|     |___ ___ ___
//  |__   | |     |     | | | | | -_| . | .'|
//  |_____|_|_|_|_|_|_|_|_|_|_|_|___|_  |__,|
//                                  |___|

// Starting state upon initialization
State state = MainMenuState;
State oldState = OffState;
boolean stateChange = false;

// Global debugging variables
boolean debugEeprom = false;
boolean debugRam = false;

//#include "spi/Touch.h"
void setup() {
	Serial.begin(115200);
	Serial.printf("\nBuild %s\n", FIRMWARE_VERSION);
	LEAK(F("Boot up"));

	Display::initialize();
	MicroSD::initialize();
	ExtEeprom::initialize();
	RamBlock::initialize();

	// Debugging, disable later
	MicroSD::test();
	ExtEeprom::test();

	LEAK(F("After init"));

	debugExternalMemory();

	//touch_init(320, 240, 0);
}


void loop() {
	//touch_touched();
	//delay(150);
	//return;


	stateChange = (state != oldState);
	oldState = state;
	switch (state) {
		// todo
		case OffState:
			break;
		case PlayState:
			break;
		case MainMenuState:
			break;
		case SpriteEditorState:
			break;
		case LogicEditorState:
			break;
		case RoomEditorState:
			break;
		case DialogEditorState:
			break;
		case DataManagementState:
			break;
		case PaletteEditorState:
			break;
		case SettingsEditorState:
			break;
		case SavedGamesState:
			break;
		default:
			break;
	}
}

/// Prints out the system's overall data structure bounds
void debugExternalMemory() {
	INFO(F("name of memory location (start, end, size)"));
	INFO(F("calibration (") << CALIB_MEMORY_START << ", " << (CALIB_MEMORY_START + CALIB_TOTAL_STORAGE_SIZE) << ", " << (CALIB_MEMORY_START + CALIB_TOTAL_STORAGE_SIZE) - CALIB_MEMORY_START << ")");
	INFO(F("color and angles (") << COLOR_MEMORY_START << ", " << (COLOR_MEMORY_START + COLOR_TOTAL_STORAGE_SIZE) << ", " << (COLOR_MEMORY_START + COLOR_TOTAL_STORAGE_SIZE) - COLOR_MEMORY_START << ")");
	INFO(F("font/glyphs (") << FONT_MEMORY_START << ", " << FONT_MEMORY_END << ", " << FONT_MEMORY_END - FONT_MEMORY_START << ")");
	INFO(F("game objects (") << objectMemoryStart << ", " << (objectMemoryStart + objectTotalStorageSize) << ", " << (objectMemoryStart + objectTotalStorageSize) - objectMemoryStart << ")");
	INFO(F("rooms (") << roomMemoryStart << ", " << (roomMemoryStart + roomTotalStorageSize) << ", " << (roomMemoryStart + roomTotalStorageSize) - roomMemoryStart << ")");
	INFO(F("dialog (") << dialogMemoryStart << ", " << (dialogMemoryStart + dialogTotalStorageSize) << ", " << (dialogMemoryStart + dialogTotalStorageSize) - dialogMemoryStart << ")");
	INFO(F("palette (") << paletteMemoryStart << ", " << (paletteMemoryStart + paletteTotalStorageSize) << ", " << (paletteMemoryStart + paletteTotalStorageSize) - paletteMemoryStart << ")");
	INFO(F("ext memory size (256k bits -> 32k bytes)"));
}