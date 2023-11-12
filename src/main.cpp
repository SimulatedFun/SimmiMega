#include <Arduino.h>
#include "spi/Display.h"
#include "spi/MicroSD.h"
#include "spi/ExtEeprom.h"

void setup(){
	Serial.begin(9600);
	Display::initialize();
	MicroSD::initialize();
	ExtEeprom::initialize();
}

void loop() {
	Display::test();
	MicroSD::test();
	ExtEeprom::test();
}