#include "Validate.h"

/// A collection of relevant functions that validate game engine data before being used.
/// The purpose is to mitigate potential crashes no matter how unlikely.
/// Mostly used for checking bounds on IDs and verifying string validity.

namespace Validate {
	boolean isUppercase(char c) {
		return (c >= 'A' and c <= 'Z');
	}

	boolean isAlphanumeric(char c) {
		return (c >= 'a' and c <= 'z') or (c >= 'A' and c <= 'Z');
	}

	boolean isDigit(char c) {
		return (c >= '0' and c <= '9');
	}

	/// Checks if a string can be used as a valid directory name
	/// \param str The string to check
	/// \return whether the string is a valid directory name
	boolean isValidDirectoryName(const char* str) {
		// check for invalid characters in the string
		for (uint32_t i = 0; i < directoryMaxLength; i++) {
			const char c = str[i];

			// letters and numbers are valid
			if (isAlphanumeric(c) || isDigit(c)) {
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
			const char validChars[] = {'-', '_', ' '};
			bool valid = false;
			for (const char validChar : validChars) {
				if (c == validChar and i != 0) {
					valid = true;
					break;
				}
			}
			if (!valid) {
				// ERROR(F("invalid char in string: ") << c << " (" << (uint8_t) c << ")");
				return false;
			}
		}
		return true;
	}

	boolean isValidGameObjectId(uint16_t id) {
		return (id < objectCount);
	}

	boolean isValidDialogId(uint8_t id) {
		return (id < dialogCount);
	}

	boolean isValidRoomId(uint8_t id) {
		return (id < roomCount);
	}

	boolean isValidCoordinates(Coordinates c) {
		return (c.x < 13 and c.y < 10 and c.roomId < roomCount);
	}

	//	/// Checks if a string is valid for use in a game's title, description, or author name
	//	/// \param str String to check
	//	/// \return whether the string is valid or not
	//	bool Storage::isValidGameMetadata(char* str) {
	//		char validChars[] = {'\0', ' ', '.', ',', '!', '?', '\'', '"', ')', '(', '-', '_', ':',
	//';'};
	//
	//		// check for invalid characters in the string
	//		for (uint32_t i = 0; i < strlen(str); i++) {
	//			char c = str[i];
	//			if (isAlphanumeric(c) || isDigit(c)) {
	//				continue;
	//			}
	//			bool valid = false;
	//			for (char validChar : validChars) {
	//				if (c == validChar) {
	//					valid = true;
	//					break;
	//				}
	//			}
	//			if (!valid) {
	//				ERROR(F("invalid char in string: ") << c << " (" << (uint8_t) c << ")");
	//				return false;
	//			}
	//		}
	//		return true;
	//	}
} // namespace Validate