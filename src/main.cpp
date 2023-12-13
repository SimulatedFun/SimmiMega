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

// Debugging variables
boolean debugEeprom = false;

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
}

void loop() {
	eeprom->test();

	{
		for (uint8_t r = 0; r < 255; r += 85) {
			for (uint8_t g = 0; g < 255; g += 85) {
				for (uint8_t b = 0; b < 255; b += 85) {
					const uint16_t color = RGB565(r, g, b);
					DrawingUtils::fill(color);
				}
			}
		}
		DrawingUtils::fillCheckerboard(RED, WHITE);
		DrawingUtils::dither(DARK_GREY, false);
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