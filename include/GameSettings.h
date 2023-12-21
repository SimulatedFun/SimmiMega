#ifndef Settings_h
#define Settings_h

#include <Arduino.h>
#include "globals/Typedefs.h"
#include "globals/DataBounds.h"
#include "Coordinates.h"
#include "GameData.h"
#include "spi/ExtEeprom.h"
#include "RamBlock.h"
#include "GameData.h"

namespace GameSettings {
	void getTitle(Title*);				  // glyph ids
	void getDescription(Description*); // glyph ids
	void getDirectory(Folder*);		  // usable directory characters
	void getStartingCoords(Coordinates* c);
	void getStartingDialog(uint8_t*);
	void setTitle(Title);				 // glyph ids
	void setDescription(Description); // glyph ids
	void setDirectory(Folder);			 // usable directory characters
	void setStartingCoords(Coordinates c);
	void setStartingDialog(uint8_t);
	uint8_t getStringLength(char* text, uint8_t maxLength);
	void clearString(char* text, uint8_t length);
	void printTitle(Title* title);
	void printDescription(Description* desc);
	void printSafe(char* text, uint8_t len);
} // namespace GameSettings

#endif