#include "main.h"
//   _____ _             _ _____
//  |   __|_|_____ _____|_|     |___ ___ ___
//  |__   | |     |     | | | | | -_| . | .'|
//  |_____|_|_|_|_|_|_|_|_|_|_|_|___|_  |__,|
//                                  |___|

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

void setup() {
	Serial.begin(115200);
	Serial.printf("Hello!\n");

	// Initialize SPI components
	{
		bus = new SPIClass(HSPI);
		bus->begin(SHARED_CLK, SHARED_MISO, SHARED_MOSI, -1);

		eeprom = new ExtEeprom(bus);
		eeprom->initialize();

		ram = new RamBlock();
		ram->initialize();

		touch = new Touch(bus);
		touch->initialize();

		display = new Display(bus);
		display->initialize();

		microSd = new MicroSD(bus);
		microSd->initialize();

		Serial.printf("Init complete\n");
	}

	//touch->calibrate();
	touch->readEepromCalibration();

	display->fillRectangle(0, 0, 320, 240, BLACK);
	//display->fillRectangle(0, 0, 320, 240, GREEN);

//	bmpDraw("/sys/data.bmp", 1, 1, 312, 240);
//	delay(1000);
}

void test() {
	microSd->test();

	eeprom->test();

	Glyphs::debugFont();

	{
		DrawingUtils::testRefreshRate();
		DrawingUtils::fillCheckerboard(RED, WHITE);
		DrawingUtils::dither(DARK_GREY, false);
		DrawingUtils::fill(BLACK);
	} // Test screen refresh rate
}

void loop() {
	touch->poll();

	// If the state changes, it runs the setup function, else it loops inside each namespace
	static boolean stateChange;
	stateChange = (state != oldState);
	oldState = state;

	switch (state) {
		case PlayState:
			//stateChange ? play::setup() : play::loop();
			break;
		case MainMenuState:
			stateChange ? MainMenu::setup() : MainMenu::loop();
			break;
		default:
			state = MainMenuState;
			oldState = OffState; // forces rerun of state change to main menu
			break;
	}
}