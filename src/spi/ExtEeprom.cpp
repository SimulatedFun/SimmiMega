#include "spi/ExtEeprom.h"

void ExtEeprom::initialize() {
	pinMode(EEPROM_CS, OUTPUT);
	pinMode(EEPROM_CS, HIGH);

	if (myEEP->init()) {
		Serial.println("EEPROM connected");
	} else {
		Serial.println("EEPROM does not respond");
		while (1)
			;
	}
	myEEP->setPageSize(EEPROM_PAGE_SIZE_64);
}

void ExtEeprom::test() {
	/* You can change the SPI clock speed. The default of is 8 MHz */
	// myEEP->setSPIClockSpeed(4000000); // use AFTER init()!

	/* Select the page size of your EEPROM.
	 * Choose EEPROM_PAGE_SIZE_xxx,
	 * with xxx = 16, 32, 64, 128 or 256
	 */

	byte byteToWrite = 42;
	myEEP->write(10, byteToWrite); // write a byte to EEPROM address 10
	byte byteToRead = myEEP->read(10);
	Serial.print("Byte read: ");
	Serial.println(byteToRead);

	int intToWrite = -4242;
	int intToRead = 0;
	myEEP->put(10, intToWrite); // write an integer to EEPROM address 10
	myEEP->get(10, intToRead);
	Serial.print("Integer read: ");
	Serial.println(intToRead);

	float floatToWrite = 42.42;
	float floatToRead = 0.0;
	myEEP->put(10, floatToWrite);
	myEEP->get(10, floatToRead);
	Serial.print("Float read: ");
	Serial.println(floatToRead);

	char charArrayToWrite[] = "This is no poetry, I am just a simple char array";
	myEEP->put(110, charArrayToWrite); // write stringToWrite to address 110
	char charArrayToRead[60] = "";	 // reserve sufficient space
	myEEP->get(110, charArrayToRead);
	Serial.print("Char array read: ");
	Serial.println(charArrayToRead);

	String stringToWrite = "Hello, I am a test string";
	unsigned int nextAddr =
			  myEEP->putString(200, stringToWrite); // String objects need a different put function

	String stringToRead = "";
	myEEP->getString(200, stringToRead);
	Serial.print("String read: ");
	Serial.println(stringToRead);
	Serial.print("Next free address: ");
	Serial.println(nextAddr);

	int intArrayToWrite[20];
	int intArrayToRead[20];
	for (unsigned int i = 0; i < (sizeof(intArrayToWrite) / sizeof(int)); i++) {
		intArrayToWrite[i] = 10 * i;
	}
	myEEP->put(250, intArrayToWrite);
	myEEP->get(250, intArrayToRead);
	for (unsigned int i = 0; i < (sizeof(intArrayToRead) / sizeof(int)); i++) {
		Serial.print("intArrayToRead[");
		Serial.print(i);
		Serial.print("]: ");
		Serial.println(intArrayToRead[i]);
	}
}

template<typename T>
void ExtEeprom::write(T data, uint16_t address, uint16_t byteLength) {
	if (address + byteLength > EEPROM_LAST_ADDRESS) {
		Serial.printf("trying to write past the max eeprom bounds: %d", address);
	}

	uint8_t* buffer = reinterpret_cast<uint8_t*>(&data);

	if (debugEeprom) {
		Serial.printf("writing (eeprom) %d bytes at address %d", byteLength, address);
	}

	for (uint16_t i = 0; i < byteLength; i++) {
		myEEP->put(address + i, buffer[i]);
	}
}
template void ExtEeprom::write(uint8_t data, uint16_t, uint16_t);

template<typename T>
void ExtEeprom::read(T* data, uint16_t address, uint16_t byteLength) {
	if (address + byteLength > EEPROM_LAST_ADDRESS) {
		Serial.printf("trying to read past the max eeprom bounds: %d", address);
	}

	uint8_t* buffer = reinterpret_cast<uint8_t*>(data);

	if (debugEeprom) {
		Serial.printf("reading (eeprom) %d bytes at address %d", byteLength, address);
	}

	for (uint16_t i = 0; i < byteLength; i++) {
		myEEP->get(address + i, buffer[i]);
	}
}
template void ExtEeprom::read(uint8_t* data, uint16_t, uint16_t);
