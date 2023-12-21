#include "Palette.h"

void Palette::save() {
	const uint32_t address = paletteMemoryStart + id * paletteStructSize;
	eeprom->write(color, address, sizeof(Color));
	ram->write(color, address, sizeof(Color));
}

void Palette::load(boolean fromRam) {
	const uint16_t address = paletteMemoryStart + id * paletteStructSize;
	if (fromRam) {
		ram->read(&color, address, sizeof(Color));
	} else {
		eeprom->read(&color, address, sizeof(Color));
		ram->write(color, address, sizeof(Color));
	}
}
