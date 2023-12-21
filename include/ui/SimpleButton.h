#ifndef SimpleButton_h
#define SimpleButton_h

#include <Arduino.h>
#include "ui/UIElement.h"
#include "Text.h"

// clang-format off
struct BtnColor {
		uint16_t mainColor;
		uint16_t pressedMainColor;
		uint16_t topColor;
		uint16_t shadowColor;
		uint16_t bgColor;

		BtnColor()
			 : mainColor(RGB565(0x937ac5)),
				pressedMainColor(RGB565(0x6547a0)),
				topColor(RGB565(0xd3c0a8)),
				shadowColor(RGB565(0x574b67)),
				bgColor(RGB565(0x2d1b2e)) {}

		BtnColor(uint16_t main, uint16_t pressed, uint16_t top, uint16_t shadow, uint16_t bg)
          : mainColor(main),
            pressedMainColor(pressed),
            topColor(top),
            shadowColor(shadow),
            bgColor(bg) {}
};

const BtnColor greenBtn =
		  {RGB565(0x8ae25d), RGB565(0x569138), RGB565(0xe4ffd6), RGB565(0x569138), RGB565(0x2d1b2e)};

const BtnColor redBtn =
		  {RGB565(0xdc423e), RGB565(0x8d2e2c), RGB565(0xffc9c8), RGB565(0x8d2e2c), RGB565(0x2d1b2e)};

const BtnColor greyBtn =
		  {RGB565(0x898989), RGB565(0x505050), RGB565(0xe1e1e1), RGB565(0x505050), RGB565(0x2d1b2e)};

const BtnColor purpleBtn =
		  {RGB565(0x9d7bc9), RGB565(0x5b4a68), RGB565(0xdbc1a8), RGB565(0x5b4a68), RGB565(0x2d1b2e)};

// clang-format on

class SimpleButton : public UIElement {
	public:
		Text* text;
		boolean value = false;
		Callback<boolean> callback;
		BtnColor color;
		SimpleButton() {
			width = 89;
			height = 28;
		}
		void render() override;
		void handlePress() override;
		void renderValue();
};
#endif