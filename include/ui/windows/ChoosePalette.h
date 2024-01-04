#ifndef ChoosePalette_h
#define ChoosePalette_h

#include <Arduino.h>
#include "spi/Display.h"
#include "Palette.h"
#include "ui/TrayElement.h"
#include "ui/RoundButton.h"
#include "ui/UIHelper.h"
#include "ui/NavigationElement.h"
#include "ui/selection-grid/Palette.h"

namespace ChoosePalette {
	void setup();
	void draw();
	void setupUI();

	void pick(boolean showZeroth, uint16_t* flagId, boolean* cancelled);
	void deallocate();

	void callbackExit(RoundButton&);
	void callbackChangeTab(uint8_t number);
	void callbackPaletteTray(uint8_t selectedPaletteId);
}

#endif