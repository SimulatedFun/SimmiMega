#ifndef Checkbox_h
#define Checkbox_h

#include <Arduino.h>
#include "ui/UIElement.h"

class Checkbox : public UIElement {
	public:
		boolean checked = false;
		Checkbox() {
			width = 19;
			height = 19;
		}
		void render() override {
			UIElement::fillInner();
			if (!checked) {
				UIElement::drawInnerBorder(true);
				return;
			}
			UIElement::drawInnerBorder(false);
			display->startWrite();
			{
				for (uint8_t i = 0; i < 6; i++) {
					display->drawHorizontalLineTx(x + 12 - i, y + 7 + i, 3, WHITE);
				}
				display->drawHorizontalLineTx(x + 13, y + 6, 2, WHITE);
				display->drawPixelTx(x + 8, y + 13, WHITE);
				display->fillRectangleTx(x + 5, y + 9, 2, 2, WHITE);
				display->fillRectangleTx(x + 6, y + 10, 2, 2, WHITE);
			}
			display->endWrite();
		}
		void handlePress() override {
			checked = !checked;
			render();
			if (callback.function != nullptr) {
				callback.function(checked);
			}
		}
		Callback<boolean> callback;
};

#endif