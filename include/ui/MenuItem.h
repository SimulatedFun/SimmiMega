#ifndef MenuItem_h
#define MenuItem_h

#include <Arduino.h>
#include "DrawingUtils.h"
#include "UIElement.h"
#include "spi/MicroSD.h"
#include "DrawBMP.h"

enum MenuShortcut {
	PlayBtn,
	SpriteBtn,
	LogicBtn,
	RoomBtn,
	TextBtn,
	PaletteBtn,
	DataBtn,
	SettingsBtn
};
class MenuItem : public UIElement {
	public:
		explicit MenuItem(MenuShortcut type, uint16_t inX, uint16_t inY) : shortcut(type) {
			x = inX;
			y = inY;
			width = 93;
			height = 74;
		}
		void render() override;
		void drawDashedBorder();
		void drawSolidBorder();
		static void drawIconBase(uint16_t xPos, uint16_t yPos, uint16_t baseColor,
										 uint16_t shadowColor);
		void handlePress() override {
			drawSolidBorder();
			if (callback.function != nullptr) {
				callback.function(*this);
			}
		}
		Callback<MenuItem&> callback;
		MenuShortcut shortcut;
};

#endif