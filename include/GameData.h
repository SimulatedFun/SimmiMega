#ifndef GameData_h
#define GameData_h

#include <Arduino.h>
#include "globals/DataBounds.h"
#include "globals/SerialDebugging.h"
#include "Validate.h"

class Folder {
	public:
		Folder() {
			resetFilename();
		}
		char text[directoryMaxLength];
		void concat() {
			text[directoryMaxLength - 1] = '\0';
		}
		boolean isValid() {
			// check for invalid characters in the string
			for (uint32_t i = 0; i < directoryMaxLength; i++) {
				const char c = text[i];

				// letters and numbers are valid
				if (Validate::isAlphanumeric(c) || Validate::isDigit(c)) {
					continue;
				}

				// if null terminator but not the first char, then valid
				if (c == '\0') {
					if (i == 0) {
						return false;
					}
					return true;
				}

				// special chars that are valid only if not at beginning
				const char validChars[] = {'-', '_'};
				bool valid = false;
				for (const char validChar : validChars) {
					if (c == validChar and i != 0) {
						valid = true;
						break;
					}
				}
				if (!valid) {
					Serial.printf("invalid char in string: %c (%d)\n", c, (uint8_t) c);
					return false;
				}
			}
			return true;
		}

		void setTo(char* src, uint8_t len) {
			if (len > directoryMaxLength or len == 0) {
				Serial.printf("bad len for file name: %d\n", len);
				resetFilename();
				return;
			}
			strlcpy(text, src, len);
		}

		void resetFilename() {
			strlcpy(text, "default", 8);
		}
};

class Title {
	public:
		char text[titleMaxLength];
		uint8_t length = 0;
		void resetTitle() {
			strlcpy(text, "Default Title", 36);
		}
		boolean isValid() { // todo
			// check for invalid characters in the string
			for (uint32_t i = 0; i < titleMaxLength; i++) {
				const char c = text[i];

				//INFO(F("title char is valid? ") << (uint8_t) c);

				// letters and numbers are valid
				if (Validate::isAlphanumeric(c) or Validate::isDigit(c)) {
					continue;
				}

				if (c == ' ') {
					continue;
				}

				if (c == '\0') {
					return true;
				}

				return false;

				// todo include other valid title characters

				//				// if null terminator but not the first char, then valid
				//				if (c == '\0') {
				//					if (i == 0) {
				//						return false;
				//					}
				//					valid = false;
				//				}
				//
				//				// special chars that are valid only if not at beginning
				//				const char validChars[] = {'-', '_', ' '};
				//				bool valid = false;
				//				for (const char validChar : validChars) {
				//					if (c == validChar and i != 0) {
				//						valid = true;
				//						break;
				//					}
				//				}
			}
			return true;
		}
};
// todo make into classes

class Description {
	public:
		char text[descriptionMaxLength];
		uint8_t length = 0;
};

#endif