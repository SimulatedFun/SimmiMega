#include "main.h"
//   _____ _             _ _____
//  |   __|_|_____ _____|_|     |___ ___ ___
//  |__   | |     |     | | | | | -_| . | .'|
//  |_____|_|_|_|_|_|_|_|_|_|_|_|___|_  |__,|
//                                  |___|

// Global objects
SPIClass* bus;
Touch* touch;
Display* display;
ExtEeprom* eeprom;
RamBlock* ram;
MicroSD* microSd;
GFXfont* currentFont;

// Debugging variables
boolean debugEeprom = true;

void setup() {
	Serial.begin(115200);
	Serial.printf("Hello!\n");

	bus = new SPIClass(HSPI);
	bus->begin(SHARED_CLK, SHARED_MISO, SHARED_MOSI, -1);

	touch = new Touch(bus);
	touch->initialize();

	display = new Display(bus);
	display->initialize();

	eeprom = new ExtEeprom(bus);
	eeprom->initialize();

	ram = new RamBlock();
	ram->initialize();

	microSd = new MicroSD(bus);
	microSd->initialize();

	test();
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
}