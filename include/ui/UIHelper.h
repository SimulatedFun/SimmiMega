#ifndef UIHelper_h
#define UIHelper_h

#include <Arduino.h>
#include "spi/Touch.h"
#include "ui/UIElement.h"
#include "globals/SerialDebugging.h"

const uint8_t maxActiveUiElements = 40;

class UIHelper {
	public:
		static void clearActive();
		static void registerUI(UIElement* ui);
		static void deregisterUI(UIElement* ui);
		static void render();
		static void loop();
		static boolean isWithinTouchBounds(UIElement* e);
		static boolean wasNewlyPressed(UIElement* e);
};

#endif