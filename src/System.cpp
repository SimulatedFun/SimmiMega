#include "System.h"

/// Loads entire game from external eeprom into ram block
void System::syncEepromAndRam() {
	uint64_t buffer = 0;
	for (uint32_t i = 0; i < RAM_EEPROM_BYTE_COUNT / 8; i++) {
		eeprom->read(&buffer, i * 8, 8);
		ram->write(buffer, i * 8, 8);
	}
	GOOD(F("synced eeprom and ram"));
}