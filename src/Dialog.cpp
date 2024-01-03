#include "Dialog.h"

// todo optimize and bring back ram versus eeprom
void Dialog::load(boolean fromRam) {
	if (dialogId >= dialogCount) {
		ERROR(F("dialog ") << dialogId << F(" OOB, cancelled load"));
		return;
	}
	const uint16_t address = dialogMemoryStart + (this->dialogId * dialogStructSize);
	for (uint8_t i = 0; i < dialogMaxLength; i++) {
		uint8_t buf = 0;
		eeprom->read(&buf, address + i, 1);
		buffer[i] = buf;
		RAW(buf << " ");
		if (buf == GLYPH_END_OF_LINE) {
			break;
		}
	}
	RAW("\n");
	calculateLength();
	boxCount = getNumBoxes();
}

void Dialog::save() {
	if (dialogId >= dialogCount) {
		ERROR(F("dialog ") << dialogId << F(" OOB, cancelled save"));
		return;
	}
	//debugRam = true;
	//debugMemory = true;
	const uint16_t address = dialogMemoryStart + (this->dialogId * dialogStructSize);
	for (uint8_t i = 0; i < dialogMaxLength; i++) {
		const uint8_t c = buffer[i];
		//RAW(c << " ");
		eeprom->write(c, address + i, 1);
		ram->write(c, address + i, 1);
		if (c == GLYPH_END_OF_LINE) {
			break;
		}
	}
	//RAW("\n");
	//debugRam = false;
	//debugMemory = false;
}

void Dialog::toString() {
	RAW("dialog " << dialogId << ":\n");
	for (const uint8_t glyphId : buffer) {
		RAW(glyphId << ", ");
	}
	RAW("\n");
}