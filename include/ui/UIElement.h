#ifndef UIElement_h
#define UIElement_h

#include <Arduino.h>
#include "spi/Display.h"
//#include "Display_bmp.h"
#include "spi/Touch.h"
#include "Callback.h"

class UIElement {
	public:
		uint8_t zIndex = 0;
		Callback<UIElement&> callback;
		UIElement() = default;
		virtual void render() = 0;
		virtual ~UIElement() = 0;
		virtual void handlePress() = 0;
		void setPosition(uint16_t inX, uint16_t inY) {
			x = inX;
			y = inY;
		}

		uint16_t x = 0, y = 0, width = 0, height = 0;
		boolean isPressed = false, oldPressed = false;
		boolean depressed = false;
		uint16_t debounceDelayMS = 250;

		uint16_t getRelativeX() {
			return touch->touchX() - x;
		}

		uint16_t getRelativeY() {
			return touch->touchY() - y;
		}

		// Draws a simple 4px padded frame around the UI object
		void drawOuterBorder(boolean glint) const;
		void drawInnerBorder(boolean grey) const;
		void fillInner() const;

		// Draws a dotted outline 1px in, 1px out of the UI element
		void drawSelected(uint16_t color) const;
};

// Captures a press anywhere on the screen
class FullScreen : public UIElement {
	public:
		FullScreen() {
			x = 0;
			y = 0;
			width = 312;
			height = 240;
		}
		void render() override {}
		void handlePress() override {
			if (callback.function != nullptr) {
				callback.function(true);
			}
		}
		Callback<boolean> callback;
};

#endif