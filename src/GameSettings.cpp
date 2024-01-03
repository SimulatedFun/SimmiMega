#include "GameSettings.h"

namespace GameSettings {
	void getTitle(Title* title) {
		eeprom->read(title, titleAddress, titleMaxLength);
		title->text[titleMaxLength - 1] = '\0';
		title->length = getStringLength(title->text, titleMaxLength);
		Serial.printf("getTitle: %s\n", title->text);
	}
	void setTitle(Title title) {
		title.text[titleMaxLength - 1] = '\0';
		eeprom->write(title, titleAddress, titleMaxLength);
	}

	void getDescription(Description* desc) {
		eeprom->read(desc, descriptionAddress, descriptionMaxLength);
		desc->text[descriptionMaxLength - 1] = '\0';
		desc->length = getStringLength(desc->text, descriptionMaxLength);
	}
	void setDescription(Description desc) {
		eeprom->write(desc, descriptionAddress, descriptionMaxLength);
	}

	void getDirectory(Folder* dir) {
		eeprom->read(dir, directoryAddress, directoryMaxLength);
		dir->concat();
		Serial.printf("getDirectory: %s\n", dir->text);
	}
	void setDirectory(Folder dir) {
		Serial.printf("setDirectory: %s\n", dir.text);
		eeprom->write(dir, directoryAddress, directoryMaxLength);
	}

	void getStartingCoords(Coordinates* c) {
		eeprom->read(c, startingCoordsAddress, startingCoordsLength);
	}
	void setStartingCoords(Coordinates c) {
		eeprom->write(c, startingCoordsAddress, startingCoordsLength);
	}

	void getStartingDialog(uint8_t* dialogId) {
		eeprom->read(dialogId, startingDialogAddress, startingDialogLength);
		Serial.printf("getStartingDialog: %d\n", *dialogId);
	}
	void setStartingDialog(uint8_t dialogId) {
		eeprom->write(dialogId, startingDialogAddress, startingDialogLength);
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
		const uint8_t length = strnlen(title->text, titleMaxLength);
		printSafe(title->text, length);
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