#ifndef ChooseXY_h
#define ChooseXY_h

#include <Arduino.h>
#include "spi/Display.h"
#include "Coordinates.h"
#include "ui/UIHelper.h"
#include "ui/selection-grid/XY.h"

namespace ChooseXY {
	void callbackXYTray(Coordinates inCoord);
	void setup();
	Coordinates pick(uint8_t roomId);
	void draw();
	void setupUI();
	void deallocate();
}

#endif