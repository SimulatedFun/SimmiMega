#ifndef EditTextField_h
#define EditTextField_h

#include <Arduino.h>
#include "spi/Display.h"
#include "Palette.h"
#include "ui/TrayElement.h"
#include "ui/RoundButton.h"
#include "ui/UIHelper.h"
#include "ui/NavigationElement.h"

constexpr uint8_t directoryMode = 0;
constexpr uint8_t titleMode = 1;
constexpr uint8_t descriptionMode = 2;

namespace EditTextField {
	void setup();
	void draw();
	void setupUI();

	boolean editField(char* inputText, uint8_t length, uint8_t mode);
	void deallocate();

	void callbackFinishEditing(RoundButton&);
	void callbackBackspace(uint8_t);
	void callbackKeyPressGlyph(uint8_t glyphId);

	void getCursor(uint16_t* x, uint16_t* y, uint16_t* color, uint8_t charIndex);
	void drawChar(uint8_t charIndex, boolean erase);
	void loopFlashingCursor();
	void refreshCursor();
}

#endif