#ifndef MainMenuState_h
#define MainMenuState_h

#include <Arduino.h>
#include "ui/MenuItem.h"
#include "globals/Async.h"
#include "states/States.h"
#include "DrawingUtils.h"
#include "ui/UIHelper.h"

namespace MainMenu {
	void setup();
	void loop();
	void draw();
	void deallocate();
	void callback_menu(MenuItem& button);
} // namespace MainMenu

#endif
