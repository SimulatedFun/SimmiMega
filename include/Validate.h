#ifndef Validate_h
#define Validate_h

#include <Arduino.h>
#include "globals/DataBounds.h"
#include "Coordinates.h"

namespace Validate {
	boolean isDigit(char c);
	boolean isAlphanumeric(char c);
	boolean isUppercase(char c);
	boolean isValidDirectoryName(const char* str);
	boolean isValidGameObjectId(uint16_t id);
	boolean isValidDialogId(uint8_t id);
	boolean isValidRoomId(uint8_t id);
	boolean isValidCoordinates(Coordinates c);
}

#endif