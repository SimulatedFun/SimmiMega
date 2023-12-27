#ifndef WriteDialogWindow_h
#define WriteDialogWindow_h

#include <Arduino.h>
#include "globals/DataBounds.h"
#include "globals/Async.h"
#include "Dialog.h"
#include "states/States.h"
#include "ui/windows/ChooseDialog.h"
#include "ui/Keyboard.h"

namespace DialogEditor {
	void setup();
	void setupUI();
	void loop();
	void loopFlashingCursor();
	void draw();

	void deallocate();
	void refreshCursor();

	void drawDialogBoxes();

	void callbackBackspace(uint8_t);
	void callbackNewline(uint8_t);
	void callbackCaps(uint8_t);
	void callbackShift(uint8_t);
	void callbackSymbols(uint8_t);
	void callbackKeyPressGlyph(uint8_t);
	void callbackExit(RoundButton&);
	void callbackClear(RoundButton&);

	void getCursor(uint16_t* x, uint16_t* y, uint16_t* color, uint8_t charIndex);
}; // namespace dialog_editor

#endif
