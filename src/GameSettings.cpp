#include "GameSettings.h"

namespace GameSettings {
	void getTitle(Title* title) {
		eeprom->read(title, titleAddress, titleMaxLength);
	}
	void setTitle(Title title) {
		eeprom->write(title, titleAddress, titleMaxLength);
		ram->write(title, titleAddress, titleMaxLength);
	}

	void getDescription(Description* desc) {
		eeprom->read(desc, descriptionAddress, descriptionMaxLength);
	}
	void setDescription(Description desc) {
		eeprom->write(desc, descriptionAddress, descriptionMaxLength);
		ram->write(desc, descriptionAddress, descriptionMaxLength);
	}

	void getDirectory(Folder* dir) {
		eeprom->read(dir, directoryAddress, directoryMaxLength);
		dir->concat();
		Serial.printf("getDirectory: %s\n", dir->text);
	}
	void setDirectory(Folder dir) {
		eeprom->write(dir, directoryAddress, directoryMaxLength);
		ram->write(dir, directoryAddress, directoryMaxLength);
	}

	void getStartingCoords(Coordinates* c) {
		eeprom->read(c, startingCoordsAddress, startingCoordsLength);
	}
	void setStartingCoords(Coordinates c) {
		eeprom->write(c, startingCoordsAddress, startingCoordsLength);
		ram->write(c, startingCoordsAddress, startingCoordsLength);
	}

	void getStartingDialog(uint8_t* dialogId) {
		eeprom->read(dialogId, startingDialogAddress, startingDialogLength);
	}
	void setStartingDialog(uint8_t dialogId) {
		eeprom->write(dialogId, startingDialogAddress, startingDialogLength);
		ram->write(dialogId, startingDialogAddress, startingDialogLength);
	}

	/// Gets the length of a string, up to the max length.
	/// Counts until it reaches a null terminator.
	uint8_t getStringLength(char* text, uint8_t maxLength) {
		uint8_t length = 0;
		for (uint8_t i = 0; i < maxLength; i++) {
			if (text[i] == '\0') {
				return length;
			}
			length++;
		}
		return maxLength;
	}

	/// Sets every character in an array to the null terminator '\0'.
	void clearString(char* text, uint8_t length) {
		for (uint8_t i = 0; i < length; i++) {
			text[i] = '\0';
		}
	}

	void printTitle(Title* title) {
		const uint8_t length = strnlen(title->text.text, titleMaxLength);
		printSafe(title->text.text, length);
	}
	void printDescription(Description* desc) {
		const uint8_t length = strnlen(desc->text, descriptionMaxLength);
		printSafe(desc->text, length);
	}
	void printSafe(char* text, uint8_t len) {
		for (uint8_t i = 0; i < len; i++) {
			if (text[i] == '\n') {
				Serial.printf("\\n");
				continue;
			}
			if (text[i] == '\0') {
				Serial.printf("\\0");
				continue;
			}
			Serial.printf("%c", text[i]);
		}
	}
} // namespace GameSettings