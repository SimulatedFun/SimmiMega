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

	eeprom->test();
	Glyphs::debugFont();
}

void loop() {
	microSd->test();

	eeprom->test();

	{
		DrawingUtils::testRefreshRate();
		DrawingUtils::fillCheckerboard(RED, WHITE);
		DrawingUtils::dither(DARK_GREY, false);
		DrawingUtils::fill(BLACK);
	} // Test screen refresh rate

	{
		uint16_t x, y, z;
		x = y = z = 0;
		touch->getRawPressure(&z);

		if (z < Z_THRESHOLD) {
			return;
		}

		touch->getRawTouch(&x, &y);
		Serial.printf("x:%d, y:%d, z:%d\n", x, y, z);
	} // Test touchscreen
}