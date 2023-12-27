#include "ui/MenuItem.h"

void MenuItem::drawDashedBorder() {
	display->startWrite();
	{
		// top and bottom borders
		for (uint8_t i = 0; i < 12; i++) {
			display->fillRectangleTx(x + 8 * i + 0, y, 4, 1, MENU_DASHED_BORDER);
			display->fillRectangleTx(x + 8 * i + 1, y + 1, 4, 1, MENU_DASHED_BORDER);

			display->fillRectangleTx(x + 8 * i + 0, y + (height - 2), 4, 1, MENU_DASHED_BORDER);
			display->fillRectangleTx(x + 8 * i + 1, y + (height - 1), 4, 1, MENU_DASHED_BORDER);
		}
		// left and right borders
		for (uint8_t i = 0; i < 9; i++) {
			display->fillRectangleTx(x + 0, y + 4 + 8 * i, 1, 4, MENU_DASHED_BORDER);
			display->fillRectangleTx(x + 1, y + 5 + 8 * i, 1, 4, MENU_DASHED_BORDER);

			display->fillRectangleTx(x + (width - 2), y + 4 + 8 * i, 1, 4, MENU_DASHED_BORDER);
			display->fillRectangleTx(x + (width - 1), y + 5 + 8 * i, 1, 4, MENU_DASHED_BORDER);
		}
	}
	display->endWrite();
}

void MenuItem::drawSolidBorder() {
	display->startWrite();
	{
		// top and bottom borders
		display->fillRectangleTx(x, y, 93, 2, WHITE);
		display->fillRectangleTx(x, y + 71, 93, 2, WHITE);

		// left and right borders
		display->fillRectangleTx(x + 0, y, 2, 69, WHITE);
		display->fillRectangleTx(x + 91, y, 2, 69, WHITE);
	}
	display->endWrite();
}

// todo reuse border styling for popup windows
void MenuItem::drawIconBase(uint16_t xPos, uint16_t yPos, uint16_t base, uint16_t shadow) {
	display->startWrite();
	{
		// top and bottom 2px borders
		display->fillRectangleTx(xPos + 2, yPos, 42, 2, BTN_PURPLE_DARK);
		display->fillRectangleTx(xPos + 2, yPos + 44, 42, 2, BTN_PURPLE_DARK);

		// left and right 2px borders
		display->fillRectangleTx(xPos, yPos + 2, 2, 42, BTN_PURPLE_DARK);
		display->fillRectangleTx(xPos + 44, yPos + 2, 2, 42, BTN_PURPLE_DARK);

		display->fillRectangleTx(xPos + 2, yPos + 2, 42, 38, base);
		display->fillRectangleTx(xPos + 4, yPos + 40, 38, 4, shadow);
		display->fillRectangleTx(xPos + 2, yPos + 38, 2, 4, shadow);
		display->fillRectangleTx(xPos + 42, yPos + 38, 2, 4, shadow);

		// corner thickness 2x2 cubes
		display->fillRectangleTx(xPos + 2, yPos + 2, 2, 2, BTN_PURPLE_DARK);
		display->fillRectangleTx(xPos + 42, yPos + 2, 2, 2, BTN_PURPLE_DARK);
		display->fillRectangleTx(xPos + 2, yPos + 42, 2, 2, BTN_PURPLE_DARK);
		display->fillRectangleTx(xPos + 42, yPos + 42, 2, 2, BTN_PURPLE_DARK);

		// inner icon 1px border
		display->drawRectangleTx(xPos + 6, yPos + 6, 34, 30, BTN_PURPLE_DARK);
	}
	display->endWrite();
}

void MenuItem::render() {
	drawDashedBorder();
	switch (shortcut) {
		case PlayBtn:
			drawIconBase(31, 16, RGB565(0xf9f4ea), RGB565(0xc5754a));
			DrawingUtils::simplePrint(30, 74, "play mode", BLACK, 1);
			drawRGBBitmap(38, 23, MenuIconPlay, 32, 28);
			break;
		case SpriteBtn:
			drawIconBase(134, 16, RGB565(0xffcd40), RGB565(0xc5754a));
			DrawingUtils::simplePrint(126, 74, "sprite editor", BLACK, 1);
			drawRGBBitmap(141, 23, MenuIconSprite, 32, 28);
			break;
		case LogicBtn:
			drawIconBase(236, 16, RGB565(0xf5ddbb), RGB565(0xc17561));
			DrawingUtils::simplePrint(232, 74, "logic editor", BLACK, 1);
			drawRGBBitmap(243, 23, MenuIconLogic, 32, 28);
			break;
		case RoomBtn:
			drawIconBase(31, 94, RGB565(0xf2d3f2), RGB565(0x9a78b4));
			DrawingUtils::simplePrint(26, 152, "room editor", BLACK, 1);
			drawRGBBitmap(38, 101, MenuIconRoom, 32, 28);
			break;
		case TextBtn:
			drawIconBase(134, 94, RGB565(0xf9f4ea), RGB565(0xc5754a));
			DrawingUtils::simplePrint(130, 152, "text editor", BLACK, 1);
			drawRGBBitmap(141, 101, MenuIconText, 32, 28);
			break;
		case PaletteBtn:
			drawIconBase(236, 94, RGB565(0xe4f2fa), RGB565(0x3491bb));
			DrawingUtils::simplePrint(223, 152, "palette editor", BLACK, 1);
			drawRGBBitmap(243, 101, MenuIconPalette, 32, 28);
			break;
		case DataBtn:
			DrawingUtils::simplePrint(32, 231, "projects", BLACK, 1);
			drawRGBBitmap(31, 173, MenuIconData, 46, 46);
			break;
		case SettingsBtn:
			drawIconBase(134, 173, RGB565(0xe9fff3), RGB565(0x0ba453));
			DrawingUtils::simplePrint(137, 231, "settings", BLACK, 1);
			drawRGBBitmap(141, 180, MenuIconSettings, 32, 28);
			break;
		default:
			break;
	}
#ifdef OUTLINE_ALL_UI
	display->drawRectangle(x, y, width, height, BLUE);
#endif
}