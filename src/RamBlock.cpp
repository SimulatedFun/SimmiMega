#include "RamBlock.h"

uint8_t block[32000] = {0};

void RamBlock::initialize() {
	for (uint16_t i = 0; i < (uint16_t)sizeof(block); i++) {
		block[i] = 0;
	}
}

template<typename T>
void RamBlock::write(T data, uint16_t address, uint16_t byteLength) {
	if (address + byteLength >= 32000) {
		Serial.printf("trying to write past the max ram bounds: %d", address);
	}

	uint8_t* buffer = reinterpret_cast<uint8_t*>(&data);

	if (debugRam) {
		Serial.printf("writing (ram) %d bytes at address %d", byteLength, address);
	}

	for (uint16_t i = 0; i < byteLength; i++) {
		block[address + i] = buffer[i];
	}
}
template void RamBlock::write(uint8_t data, uint16_t, uint16_t);

template<typename T>
void RamBlock::read(T* data, uint16_t address, uint16_t byteLength) {
	if (address + byteLength >= 32000) {
		Serial.printf("trying to read past the max ram bounds: %d", address);
	}

	uint8_t* buffer = reinterpret_cast<uint8_t*>(data);

	if (debugRam) {
		Serial.printf("reading (ram) %d bytes at address %d", byteLength, address);
	}

	for (uint16_t i = 0; i < byteLength; i++) {
		buffer[i] = block[address + i];
	}
}
template void RamBlock::read(uint8_t* data, uint16_t, uint16_t);