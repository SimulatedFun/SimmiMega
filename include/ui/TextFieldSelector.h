#ifndef TextField_h
#define TextField_h

#include <Arduino.h>
#include "spi/Display.h"
#include "DrawBMP.h"
#include "spi/Touch.h"
#include "globals/DataBounds.h"
#include "Callback.h"
#include "UIElement.h"
#include "Text.h"

/// Todo: consider consolidating the text field selector and the dialog preview ui elements
class TextFieldSelector : public UIElement {
	public:
		Text* text = nullptr;
		Callback<boolean> callback;
		TextFieldSelector() {
			width = 184;
			height = 24;
		}
		void render() override {
			UIElement::drawInnerBorder(false);
			UIElement::fillInner();
			renderText();
		}
		void handlePress() override {
			if (callback.function != nullptr) {
				callback.function(true);
			}
		}
		void renderText() {
			if (text == nullptr) {
				return;
			}
			text->setPosition(x + 8, y + 17);
			text->setStyle(1, WHITE);
			text->print();
		}
};

// just for drawing the text area, nothing more
class TextField : public UIElement {
	public:
		//char* text;
		TextField() {
			width = 248;
			height = 80;
		}
		void render() override {
			UIElement::drawInnerBorder(false);
			UIElement::fillInner();
		}
		void handlePress() override {}
};

#endif