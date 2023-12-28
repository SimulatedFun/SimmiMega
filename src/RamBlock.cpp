#include "RamBlock.h"

uint8_t block[RAM_BYTE_LEN] = {0};

void RamBlock::initialize() {
	clear();
}

void RamBlock::clear() {
	for (uint16_t i = 0; i < RAM_BYTE_LEN; i++) {
		block[i] = 0;
	}
}

template<typename T>
void RamBlock::write(T data, uint16_t address, uint16_t byteLength) {
	if (address + byteLength > RAM_BYTE_LEN) {
		Serial.printf("trying to write past the max ram bounds: %d\n", address);
	}

	uint8_t* buffer = reinterpret_cast<uint8_t*>(&data);

	if (debugRam) {
		Serial.printf("writing (ram) %d bytes at address %d\n", byteLength, address);
	}

	for (uint16_t i = 0; i < byteLength; i++) {
		block[address + i] = buffer[i];
	}
}
template void RamBlock::write(uint8_t data, uint16_t, uint16_t);
template void RamBlock::write(uint16_t data, uint16_t, uint16_t);
template void RamBlock::write(uint32_t data, uint16_t, uint16_t);
template void RamBlock::write(uint64_t data, uint16_t, uint16_t);
template void RamBlock::write(Object data, uint16_t, uint16_t);
template void RamBlock::write(Color data, uint16_t, uint16_t);
template void RamBlock::write(Coordinates data, uint16_t, uint16_t);
template void RamBlock::write(Title data, uint16_t, uint16_t);
template void RamBlock::write(Description data, uint16_t, uint16_t);
template void RamBlock::write(Folder data, uint16_t, uint16_t);

template<typename T>
void RamBlock::read(T* data, uint16_t address, uint16_t byteLength) {
	if (address + byteLength > RAM_BYTE_LEN) {
		Serial.printf("trying to read past the max ram bounds: %d\n", address);
	}

	uint8_t* buffer = reinterpret_cast<uint8_t*>(data);

	if (debugRam) {
		Serial.printf("reading (ram) %d bytes at address %d\n", byteLength, address);
	}

	for (uint16_t i = 0; i < byteLength; i++) {
		buffer[i] = block[address + i];
	}
}
template void RamBlock::read(uint8_t* data, uint16_t, uint16_t);
template void RamBlock::read(uint16_t* data, uint16_t, uint16_t);
template void RamBlock::read(uint32_t* data, uint16_t, uint16_t);
template void RamBlock::read(uint64_t* data, uint16_t, uint16_t);
template void RamBlock::read(Object* data, uint16_t, uint16_t);
template void RamBlock::read(Color* data, uint16_t, uint16_t);
template void RamBlock::read(Coordinates* data, uint16_t, uint16_t);
