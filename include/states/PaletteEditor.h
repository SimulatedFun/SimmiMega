#ifndef ColorSelectorWindow_h
#define ColorSelectorWindow_h

#include <Arduino.h>
#include "globals/DataBounds.h"
#include "Palette.h"
#include "spi/Display.h"
#include "states/States.h"
#include "ui/windows/ChoosePalette.h"
#include "ui/ValueButton.h"

namespace PaletteEditor {
	void setup();
	void setupUI();
	void loop();
	void draw();

	void deallocate();

	void callbackSelectDepth(uint8_t depth);
	void callbackSelectColor(uint16_t color);
	void callbackSelectBrightness(uint8_t inBrightness);
	void callbackExit(RoundButton&);
} // namespace PaletteEditor

#endif
