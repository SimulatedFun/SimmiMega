#ifndef ChooseRoom_h
#define ChooseRoom_h

#include <Arduino.h>
#include "spi/Display.h"
#include "ui/UIHelper.h"
#include "ui/NavigationElement.h"
#include "ui/selection-grid/Room.h"

namespace ChooseRoom {
	void callbackRoomTray(unsigned int selectedRoomId);
	void setup();
	uint8_t pick();
	void draw();
	void setupUI();
	void deallocate();
} // namespace ChooseRoom

#endif