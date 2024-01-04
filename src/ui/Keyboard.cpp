#include "ui/Keyboard.h"

// region Key Positions
// clang-format off
constexpr uint8_t normalKeyCount = 45;
const KeyPosition positions[normalKeyCount] {
	// first row
	{5 + 25*0, 130, '1', GlyphKey, '/', GlyphKey, GLYPH_COLOR_1, ColorKey},
	{5 + 25*1, 130, '2', GlyphKey, '@', GlyphKey, GLYPH_COLOR_2, ColorKey},
	{5 + 25*2, 130, '3', GlyphKey, '#', GlyphKey, GLYPH_COLOR_3, ColorKey},
	{5 + 25*3, 130, '4', GlyphKey, '$', GlyphKey, GLYPH_COLOR_4, ColorKey},
	{5 + 25*4, 130, '5', GlyphKey, '%', GlyphKey, GLYPH_COLOR_5, ColorKey},
	{5 + 25*5, 130, '6', GlyphKey, '^', GlyphKey, 0, GlyphKey},
	{5 + 25*6, 130, '7', GlyphKey, '&', GlyphKey, 0, GlyphKey},
	{5 + 25*7, 130, '8', GlyphKey, '*', GlyphKey, 0, GlyphKey},
	{5 + 25*8, 130, '9', GlyphKey, '(', GlyphKey, 0, GlyphKey},
	{5 + 25*9, 130, '0', GlyphKey, ')', GlyphKey, 0, GlyphKey},
	{5 + 25*10, 130, '-', GlyphKey, '_', GlyphKey, 0, GlyphKey},
	{5 + 25*11, 130, '=', GlyphKey, '=', GlyphKey, 0, GlyphKey}, // todo shift is same char

		  // second row
	{18 + 25*0, 152, 'q', GlyphKey, 'Q', GlyphKey, GLYPH_COLOR_6, ColorKey},
	{18 + 25*1, 152, 'w', GlyphKey, 'W', GlyphKey, GLYPH_COLOR_7, ColorKey},
	{18 + 25*2, 152, 'e', GlyphKey, 'E', GlyphKey, GLYPH_COLOR_8, ColorKey},
	{18 + 25*3, 152, 'r', GlyphKey, 'R', GlyphKey, GLYPH_COLOR_9, ColorKey},
	{18 + 25*4, 152, 't', GlyphKey, 'T', GlyphKey, GLYPH_COLOR_10, ColorKey},
	{18 + 25*5, 152, 'y', GlyphKey, 'Y', GlyphKey, 0, GlyphKey},
	{18 + 25*6, 152, 'u', GlyphKey, 'U', GlyphKey, 0, GlyphKey},
	{18 + 25*7, 152, 'i', GlyphKey, 'I', GlyphKey, 0, GlyphKey},
	{18 + 25*8, 152, 'o', GlyphKey, 'O', GlyphKey, 0, GlyphKey},
	{18 + 25*9, 152, 'p', GlyphKey, 'P', GlyphKey, 0, GlyphKey},

	// third row
	{31 + 25*0, 174, 'a', GlyphKey, 'A', GlyphKey, GLYPH_COLOR_11, ColorKey},
	{31 + 25*1, 174, 's', GlyphKey, 'S', GlyphKey, GLYPH_COLOR_12, ColorKey},
	{31 + 25*2, 174, 'd', GlyphKey, 'D', GlyphKey, GLYPH_COLOR_13, ColorKey},
	{31 + 25*3, 174, 'f', GlyphKey, 'F', GlyphKey, GLYPH_COLOR_14, ColorKey},
	{31 + 25*4, 174, 'g', GlyphKey, 'G', GlyphKey, GLYPH_COLOR_15, ColorKey},
	{31 + 25*5, 174, 'h', GlyphKey, 'H', GlyphKey, 0, GlyphKey},
	{31 + 25*6, 174, 'j', GlyphKey, 'J', GlyphKey, 0, GlyphKey},
	{31 + 25*7, 174, 'k', GlyphKey, 'K', GlyphKey, 0, GlyphKey},
	{31 + 25*8, 174, 'l', GlyphKey, 'L', GlyphKey, 0, GlyphKey},

	// fourth row
	{44 + 25*0, 196, 'z', GlyphKey, 'Z', GlyphKey, 0, GlyphKey},
	{44 + 25*1, 196, 'x', GlyphKey, 'X', GlyphKey, 0, GlyphKey},
	{44 + 25*2, 196, 'c', GlyphKey, 'C', GlyphKey, 0, GlyphKey},
	{44 + 25*3, 196, 'v', GlyphKey, 'V', GlyphKey, 0, GlyphKey},
	{44 + 25*4, 196, 'b', GlyphKey, 'B', GlyphKey, 0, GlyphKey},
	{44 + 25*5, 196, 'n', GlyphKey, 'N', GlyphKey, 0, GlyphKey},
	{44 + 25*6, 196, 'm', GlyphKey, 'M', GlyphKey, 0, GlyphKey},
	{44 + 25*7, 196, ',', GlyphKey, ',', GlyphKey, 0, GlyphKey},
	{44 + 25*8, 196, '.', GlyphKey, '.', GlyphKey, 0, GlyphKey},
	{44 + 25*9, 196, '/', GlyphKey, '/', GlyphKey, 0, GlyphKey},

	{55 + 25*0, 218, ';', GlyphKey, ';', GlyphKey, 0, GlyphKey},
	{55 + 25*1, 218, '\'', GlyphKey, '\'', GlyphKey, 0, GlyphKey},

	{230 + 25*0, 218, '!', GlyphKey, '!', GlyphKey, 0, GlyphKey},
	{230 + 25*1, 218, '?', GlyphKey, '?', GlyphKey, 0, GlyphKey}
};

constexpr uint8_t directoryModeKeyCount = 38;
const KeyPosition directoryKeyboardLayout[directoryModeKeyCount] {
	// first row
	{5 + 25*0, 130, '1', GlyphKey, '1', GlyphKey, ' ', GlyphKey},
	{5 + 25*1, 130, '2', GlyphKey, '2', GlyphKey, ' ', GlyphKey},
	{5 + 25*2, 130, '3', GlyphKey, '3', GlyphKey, ' ', GlyphKey},
	{5 + 25*3, 130, '4', GlyphKey, '4', GlyphKey, ' ', GlyphKey},
	{5 + 25*4, 130, '5', GlyphKey, '5', GlyphKey, ' ', GlyphKey},
	{5 + 25*5, 130, '6', GlyphKey, '6', GlyphKey, ' ', GlyphKey},
	{5 + 25*6, 130, '7', GlyphKey, '7', GlyphKey, ' ', GlyphKey},
	{5 + 25*7, 130, '8', GlyphKey, '8', GlyphKey, ' ', GlyphKey},
	{5 + 25*8, 130, '9', GlyphKey, '9', GlyphKey, ' ', GlyphKey},
	{5 + 25*9, 130, '0', GlyphKey, '0', GlyphKey, ' ', GlyphKey},
	{5 + 25*10, 130, '-', GlyphKey, '-', GlyphKey, ' ', GlyphKey},
	{5 + 25*11, 130, '_', GlyphKey, '_', GlyphKey, ' ', GlyphKey},

	// second row
	{18 + 25*0, 152, 'q', GlyphKey, 'Q', GlyphKey, ' ', GlyphKey},
	{18 + 25*1, 152, 'w', GlyphKey, 'W', GlyphKey, ' ', GlyphKey},
	{18 + 25*2, 152, 'e', GlyphKey, 'E', GlyphKey, ' ', GlyphKey},
	{18 + 25*3, 152, 'r', GlyphKey, 'R', GlyphKey, ' ', GlyphKey},
	{18 + 25*4, 152, 't', GlyphKey, 'T', GlyphKey, ' ', GlyphKey},
	{18 + 25*5, 152, 'y', GlyphKey, 'Y', GlyphKey, ' ', GlyphKey},
	{18 + 25*6, 152, 'u', GlyphKey, 'U', GlyphKey, ' ', GlyphKey},
	{18 + 25*7, 152, 'i', GlyphKey, 'I', GlyphKey, ' ', GlyphKey},
	{18 + 25*8, 152, 'o', GlyphKey, 'O', GlyphKey, ' ', GlyphKey},
	{18 + 25*9, 152, 'p', GlyphKey, 'P', GlyphKey, ' ', GlyphKey},

	// third row
	{31 + 25*0, 174, 'a', GlyphKey, 'A', GlyphKey, ' ', GlyphKey},
	{31 + 25*1, 174, 's', GlyphKey, 'S', GlyphKey, ' ', GlyphKey},
	{31 + 25*2, 174, 'd', GlyphKey, 'D', GlyphKey, ' ', GlyphKey},
	{31 + 25*3, 174, 'f', GlyphKey, 'F', GlyphKey, ' ', GlyphKey},
	{31 + 25*4, 174, 'g', GlyphKey, 'G', GlyphKey, ' ', GlyphKey},
	{31 + 25*5, 174, 'h', GlyphKey, 'H', GlyphKey, ' ', GlyphKey},
	{31 + 25*6, 174, 'j', GlyphKey, 'J', GlyphKey, ' ', GlyphKey},
	{31 + 25*7, 174, 'k', GlyphKey, 'K', GlyphKey, ' ', GlyphKey},
	{31 + 25*8, 174, 'l', GlyphKey, 'L', GlyphKey, ' ', GlyphKey},

	// fourth row
	{44 + 25*0, 196, 'z', GlyphKey, 'Z', GlyphKey, ' ', GlyphKey},
	{44 + 25*1, 196, 'x', GlyphKey, 'X', GlyphKey, ' ', GlyphKey},
	{44 + 25*2, 196, 'c', GlyphKey, 'C', GlyphKey, ' ', GlyphKey},
	{44 + 25*3, 196, 'v', GlyphKey, 'V', GlyphKey, ' ', GlyphKey},
	{44 + 25*4, 196, 'b', GlyphKey, 'B', GlyphKey, ' ', GlyphKey},
	{44 + 25*5, 196, 'n', GlyphKey, 'N', GlyphKey, ' ', GlyphKey},
	{44 + 25*6, 196, 'm', GlyphKey, 'M', GlyphKey, ' ', GlyphKey},
};
// clang-format on
// endregion

void Keyboard::handlePress() {
	uint8_t count = 0;
	const KeyPosition* start = nullptr;

	if (mode == NormalKeyboard) {
		start = positions;
		count = normalKeyCount;
	} else if (mode == DirectoryMode) {
		start = directoryKeyboardLayout;
		count = directoryModeKeyCount;
	}

	for (uint8_t i = 0; i < count; i++) {
		KeyPosition pos;
		memcpy(&pos, &start[i], sizeof(KeyPosition));
		INFO("keyposition: (" << pos.x << ", " << pos.y << "), value: " << pos.value);
		setKeyData(pos);
		if (UIHelper::isWithinTouchBounds(key)) {
			key->handlePress();

			// Un-press shift after a letter is typed
			if (shift->depressed) {
				shift->depressed = false;
				shift->render();
				renderKeys();
			}
			return;
		}
	}
}

void Keyboard::setKeyData(KeyPosition pos) {
	key->setPosition(pos.x, pos.y);

	switch (mode) {
		case NormalKeyboard:
			if (shift->depressed or caps->depressed) {
				key->type = pos.shiftType;
				if (pos.shiftType == GlyphKey) {
					key->glyphIdValue = Text::asciiToGlyphLookupId((char)pos.shiftValue);
				} else {
					key->glyphIdValue = pos.shiftValue;
				}

			} else {
				key->type = pos.type;
				if (pos.type == GlyphKey) {
					key->glyphIdValue = Text::asciiToGlyphLookupId(pos.value);
				} else {
					key->glyphIdValue = pos.value;
				}
			}

			break;
		case SymbolKeyboard: // todo add colors and symbols
			key->glyphIdValue = pos.secondaryValue;
			key->type = pos.secondaryType;
			break;
		case DirectoryMode:
			key->glyphIdValue = Text::asciiToGlyphLookupId(pos.value);
			key->type = pos.type;
			break;
	}
}

void Keyboard::renderKeys() {
	uint8_t count = 0;
	const KeyPosition* start = nullptr;

	if (mode == NormalKeyboard or mode == SymbolKeyboard) {
		start = positions;
		count = normalKeyCount;
	} else if (mode == DirectoryMode) {
		start = directoryKeyboardLayout;
		count = directoryModeKeyCount;
	}

	for (uint8_t i = 0; i < count; i++) {
		KeyPosition pos;
		memcpy(&pos, &start[i], sizeof(KeyPosition));
		setKeyData(pos);
		key->render();
	}
}

void KeyboardKey::render() {
	uint16_t mainColor = PINK_COLOR;
	uint16_t pressedMainColor = PINK_COLOR;
	uint16_t topColor = PINK_COLOR;
	uint16_t shadowColor = PINK_COLOR;
	const uint16_t bgColor = RGB565(0x2d1b2e);
	switch (type) {
		case GlyphKey:
		case ColorKey:
		case SpaceKey:
			mainColor = RGB565(0x937ac5);
			pressedMainColor = RGB565(0x6547a0);
			topColor = RGB565(0xd3c0a8);
			shadowColor = RGB565(0x574b67);
			break;
		case BackspaceKey:
		case EnterKey:
		case ShiftKey:
		case CapsKey:
			mainColor = RGB565(0xf7db53);
			pressedMainColor = RGB565(0xc5754a);
			topColor = RGB565(0xf9f4ea);
			shadowColor = RGB565(0xc5754a);
			break;
		case SymbolsKey:
			mainColor = RGB565(0xd3c0a8);
			pressedMainColor = RGB565(0x7b6e5d);
			topColor = RGB565(0xd3c0a8);
			shadowColor = RGB565(0x7b6e5d);
			break;
	}
	if (!depressed) {
		display->startWrite();
		{
			// inner light purple
			display->fillRectangleTx(x + 1, y + 1, width - 2, height - 4, mainColor);

			// top tan
			display->drawHorizontalLineTx(x + 1, y, width - 3, topColor);
			display->drawVerticalLineTx(x, y + 1, height - 5, topColor);

			// bottom shadow
			display->fillRectangleTx(x + 1, y + height - 3, width - 3, 2, shadowColor);
			display->drawVerticalLineTx(x + width - 2, y + height - 4, 2, shadowColor);
			display->drawVerticalLineTx(x, y + height - 4, 2, shadowColor);

			// outer dark purple
			display->drawVerticalLineTx(x + width - 1, y, height, bgColor);
			display->drawHorizontalLineTx(x, y + height - 1, width, bgColor);
			display->drawPixelTx(x, y, bgColor);
			display->drawPixelTx(x, y + height - 2, bgColor);
			display->drawPixelTx(x + width - 2, y, bgColor);
			display->drawPixelTx(x + width - 2, y + height - 2, bgColor);
		}
		display->endWrite();
	} else {
		display->startWrite();
		{
			// inner purple
			display->fillRectangleTx(x, y + 1, width - 2, height - 3, pressedMainColor);

			// top bg
			display->drawHorizontalLineTx(x + 1, y, width - 3, bgColor);

			// bottom tan
			display->drawHorizontalLineTx(x + 1, y + height - 2, width - 3, topColor);
			display->drawVerticalLineTx(x + width - 2, y + 2, height - 4, topColor);

			// top left
			display->drawPixelTx(x, y + 1, bgColor);
		}
		display->endWrite();
	}
	renderValue();
}

uint16_t Keyboard::glyphToColor(uint8_t glyphId) {
	switch (glyphId) {
		case GLYPH_COLOR_1:
			return RGB565(0xaa0000);
		case GLYPH_COLOR_2:
			return RGB565(0xff5555);
		case GLYPH_COLOR_3:
			return RGB565(0xaa00aa);
		case GLYPH_COLOR_4:
			return RGB565(0xff55ff);
		case GLYPH_COLOR_5:
			return RGB565(0x555555);
		case GLYPH_COLOR_6:
			return RGB565(0x0000aa);
		case GLYPH_COLOR_7:
			return RGB565(0x5555ff);
		case GLYPH_COLOR_8:
			return RGB565(0x00aaaa);
		case GLYPH_COLOR_9:
			return RGB565(0x55ffff);
		case GLYPH_COLOR_10:
			return RGB565(0xaaaaaa);
		case GLYPH_COLOR_11:
			return RGB565(0x00aa00);
		case GLYPH_COLOR_12:
			return RGB565(0x55ff55);
		case GLYPH_COLOR_13:
			return RGB565(0xffaa00);
		case GLYPH_COLOR_14:
			return RGB565(0xffff54);
		case GLYPH_COLOR_15:
			return WHITE;
	}
	return PINK_COLOR;
}

void KeyboardKey::renderValue() {
	uint8_t offset = 0;
	if (depressed) {
		offset = 1;
	}
	if (type == GlyphKey) {
		const uint8_t pixelWidth = Glyphs::getGlyphPixelWidth(glyphIdValue, 1);
		const uint8_t availableWidth = 22;
		const uint16_t padding = (availableWidth / 2) - (pixelWidth / 2);
		display->startWrite();
		{
			Glyphs::drawGlyphTx(x + 2 + padding + offset, y + 15 + offset, glyphIdValue,
									  RGB565(0x574b67), 1);
			Glyphs::drawGlyphTx(x + 1 + padding + offset, y + 14 + offset, glyphIdValue, WHITE, 1);
		}
		display->endWrite();
	} else if (type == ColorKey) {
		display->startWrite();
		display->fillRectangleTx(x + 3 + offset, y + 3 + offset, 19, 14,
										 Keyboard::glyphToColor(glyphIdValue));
		display->endWrite();
	} else if (type == BackspaceKey) {
		display->startWrite();
		Glyphs::drawGlyphTx(x + 15 + offset, y + 22 + offset, KB_BACKSPACE_ARROW, BLACK, 1);
		display->endWrite();
	} else if (type == EnterKey) {
		display->startWrite();
		Glyphs::drawGlyphTx(x + 11 + offset, y + 21 + offset, KB_ENTER_ARROW, BLACK, 1);
		display->endWrite();
	} else if (type == SymbolsKey) {
		const uint16_t whiteColor = depressed ? RGB565(0xffffff) : WHITE;
		display->startWrite();
		{
			Glyphs::drawGlyphTx(x + 8 + offset, y + 20 + offset, KB_PALETTE_OUTLINE, RGB565(0x574b67),
									  1);
			Glyphs::drawGlyphTx(x + 8 + offset, y + 20 + offset, KB_PALETTE_LEFT, CMYK_CYAN, 1);
			Glyphs::drawGlyphTx(x + 8 + offset, y + 20 + offset, KB_PALETTE_RIGHT, CMYK_MAGENTA, 1);
			Glyphs::drawGlyphTx(x + 8 + offset, y + 20 + offset, KB_PALETTE_TOP, CMYK_YELLOW, 1);
			Glyphs::drawGlyphTx(x + 22 + offset, y + 20 + offset, KB_SMILEY_OUTLINE, RGB565(0x574b67),
									  1);
			Glyphs::drawGlyphTx(x + 22 + offset, y + 20 + offset, KB_SMILEY_WHITE, whiteColor, 1);
			Glyphs::drawGlyphTx(x + 36 + offset, y + 20 + offset, KB_HEART_OUTLINE, RGB565(0x574b67),
									  1);
			Glyphs::drawGlyphTx(x + 36 + offset, y + 20 + offset, KB_HEART_WHITE, whiteColor, 1);
		}
		display->endWrite();
	} else if (type == ShiftKey) {
		// todo small font for "space" "shift" "enter" ?
	} else if (type == CapsKey) {
	} else if (type == SpaceKey) {
	}
}

void KeyboardKey::handlePress() {
	switch (type) {
		case GlyphKey:
		case ColorKey:
		case SpaceKey:
		case SymbolsKey:
		case BackspaceKey:
		case EnterKey:
			depressed = true; // key can't handle its own state
			render();
			if (callback.function != nullptr) {
				callback.function(glyphIdValue);
			}
			delay(100);
			depressed = false;
			render();
			break;
		case CapsKey:
		case ShiftKey:
			depressed = !depressed;
			render();
			if (callback.function != nullptr) {
				callback.function(glyphIdValue);
			}
			break;
	}
}
