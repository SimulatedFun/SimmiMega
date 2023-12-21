#include "ui/RoundButton.h"

void RoundButton::drawUnPressed() {
	uint16_t mainColor = RGB565(0x937ac5);
	uint16_t shadowColor = RGB565(0x574b67);
	uint16_t darkColor = RGB565(0x2d1b2e);
	uint16_t tanColor = RGB565(0xffeecc);
	if (identifier == CheckmarkButton) {
		mainColor = RGB565(0x8ae25d);
		shadowColor = RGB565(0x3b8367);
	}
	display->startWrite();
	{
		// dark outer purple
		display->fillRectangleTx(x, y, 24, 24, darkColor);

		// main light purple color
		display->fillRectangleTx(x + 2, y + 2, 19, 18, mainColor);
		display->drawHorizontalLineTx(x + 4, y + 1, 15, mainColor);
		display->drawHorizontalLineTx(x + 3, y + 20, 17, mainColor);
		display->drawVerticalLineTx(x + 1, y + 4, 15, mainColor);
		display->drawVerticalLineTx(x + 21, y + 3, 16, mainColor);
		display->drawVerticalLineTx(x + 22, y + 4, 13, mainColor);

		// tan/white top and left borders
		display->drawHorizontalLineTx(x + 2, y + 1, 2, tanColor);
		display->drawHorizontalLineTx(x + 4, y, 15, tanColor);
		display->drawVerticalLineTx(x, y + 4, 13, tanColor);
		display->drawVerticalLineTx(x + 1, y + 2, 2, tanColor);
		display->drawHorizontalLineTx(x + 19, y + 1, 2, tanColor);
		display->drawPixelTx(x + 21, y + 2, tanColor);

		// under shadow
		display->fillRectangleTx(x + 4, y + 21, 15, 2, shadowColor);
		display->drawVerticalLineTx(x, y + 17, 2, shadowColor);
		display->drawVerticalLineTx(x + 22, y + 17, 2, shadowColor);
		display->drawVerticalLineTx(x + 1, y + 19, 2, shadowColor);
		display->drawVerticalLineTx(x + 21, y + 19, 2, shadowColor);
		display->drawVerticalLineTx(x + 2, y + 20, 2, shadowColor);
		display->drawVerticalLineTx(x + 20, y + 20, 2, shadowColor);
		display->drawPixelTx(x + 3, y + 21, shadowColor);
		display->drawPixelTx(x + 19, y + 21, shadowColor);
	}
	display->endWrite();
}

void RoundButton::drawPressed() {
	uint16_t mainColor = RGB565(0x6547a0);
	uint16_t shadowColor = RGB565(0x574b67);
	uint16_t darkColor = RGB565(0x2d1b2e);
	uint16_t tanColor = RGB565(0xffeecc);
	if (identifier == CheckmarkButton) {
		mainColor = RGB565(0x3eb800);
		shadowColor = RGB565(0x3b8367);
	}
	display->startWrite();
	{
		// dark outer purple
		display->fillRectangleTx(x, y, width, height, darkColor);

		// main light purple color
		display->fillRectangleTx(x + 2, y + 2, 19, 18, mainColor);
		display->drawHorizontalLineTx(x + 4, y + 1, 15, mainColor);
		display->drawHorizontalLineTx(x + 3, y + 20, 17, mainColor);
		display->drawVerticalLineTx(x + 1, y + 4, 15, mainColor);
		display->drawVerticalLineTx(x + 21, y + 3, 16, mainColor);
		display->drawVerticalLineTx(x + 22, y + 4, 13, mainColor);

		// tan/white top and left borders
		display->drawHorizontalLineTx(x + 2, y + 1, 2, tanColor);
		display->drawHorizontalLineTx(x + 4, y, 15, tanColor);
		display->drawVerticalLineTx(x, y + 4, 13, tanColor);
		display->drawVerticalLineTx(x + 1, y + 2, 2, tanColor);
		display->drawHorizontalLineTx(x + 19, y + 1, 2, tanColor);
		display->drawPixelTx(x + 21, y + 2, tanColor);

		// under shadow
		display->fillRectangleTx(x + 4, y + 21, 15, 2, shadowColor);
		display->drawVerticalLineTx(x, y + 17, 2, shadowColor);
		display->drawVerticalLineTx(x + 22, y + 17, 2, shadowColor);
		display->drawVerticalLineTx(x + 1, y + 19, 2, shadowColor);
		display->drawVerticalLineTx(x + 21, y + 19, 2, shadowColor);
		display->drawVerticalLineTx(x + 2, y + 20, 2, shadowColor);
		display->drawVerticalLineTx(x + 20, y + 20, 2, shadowColor);
		display->drawPixelTx(x + 3, y + 21, shadowColor);
		display->drawPixelTx(x + 19, y + 21, shadowColor);
	}
	display->endWrite();
}

void RoundButton::drawIcon(boolean pressed) {
	display->startWrite();
	{
		switch (identifier) {
			case ExitButton:
				if (pressed) {
					Glyphs::drawGlyphTx(x + 4, y + 20, EXIT_OUTLINE, RGB565(0x574b67), 1);
					Glyphs::drawGlyphTx(x + 4, y + 20, EXIT_WHITE, RGB565(0xa39999), 1);
				} else {
					Glyphs::drawGlyphTx(x + 4, y + 19, EXIT_OUTLINE, RGB565(0x574b67), 1);
					Glyphs::drawGlyphTx(x + 4, y + 19, EXIT_WHITE, RGB565(0xffffff), 1);
				}
				break;
			case PencilButton:
				if (pressed) {
					Glyphs::drawGlyphTx(x + 5, y + 20, PENCIL_OUTLINE, RGB565(0x574b67), 1);
					Glyphs::drawGlyphTx(x + 5, y + 20, PENCIL_WHITE, RGB565(0xa39999), 1);
					Glyphs::drawGlyphTx(x + 5, y + 20, PENCIL_SHADOW, RGB565(0xa39999), 1);
				} else {
					Glyphs::drawGlyphTx(x + 5, y + 19, PENCIL_OUTLINE, RGB565(0x574b67), 1);
					Glyphs::drawGlyphTx(x + 5, y + 19, PENCIL_WHITE, RGB565(0xffffff), 1);
					Glyphs::drawGlyphTx(x + 5, y + 19, PENCIL_SHADOW, RGB565(0xd3c0a8), 1);
				}
				break;
			case EraserButton:
				if (pressed) {
					Glyphs::drawGlyphTx(x + 5, y + 23, ERASER_OUTLINE, RGB565(0x574b67), 1);
					Glyphs::drawGlyphTx(x + 5, y + 23, ERASER_WHITE, RGB565(0xa39999), 1);
					Glyphs::drawGlyphTx(x + 5, y + 23, ERASER_SHADOW, RGB565(0xa39999), 1);
				} else {
					Glyphs::drawGlyphTx(x + 5, y + 22, ERASER_OUTLINE, RGB565(0x574b67), 1);
					Glyphs::drawGlyphTx(x + 5, y + 22, ERASER_WHITE, RGB565(0xffffff), 1);
					Glyphs::drawGlyphTx(x + 5, y + 22, ERASER_SHADOW, RGB565(0xd3c0a8), 1);
				}
				break;
			case CopyButton:
				if (pressed) {
					Glyphs::drawGlyphTx(x + 5, y + 20, COPY_OUTLINE, RGB565(0x574b67), 1);
					Glyphs::drawGlyphTx(x + 5, y + 20, COPY_WHITE, RGB565(0xa39999), 1);
					Glyphs::drawGlyphTx(x + 5, y + 20, COPY_SHADOW, RGB565(0xa39999), 1);
				} else {
					Glyphs::drawGlyphTx(x + 5, y + 19, COPY_OUTLINE, RGB565(0x574b67), 1);
					Glyphs::drawGlyphTx(x + 5, y + 19, COPY_WHITE, RGB565(0xffffff), 1);
					Glyphs::drawGlyphTx(x + 5, y + 19, COPY_SHADOW, RGB565(0xd3c0a8), 1);
				}
				break;
			case PasteButton:
				if (pressed) {
					Glyphs::drawGlyphTx(x + 4, y + 19, PASTE_OUTLINE, RGB565(0x574b67), 1);
					Glyphs::drawGlyphTx(x + 4, y + 19, PASTE_WHITE, RGB565(0xa39999), 1);
					Glyphs::drawGlyphTx(x + 4, y + 19, PASTE_SHADOW, RGB565(0xa39999), 1);
				} else {
					Glyphs::drawGlyphTx(x + 4, y + 18, PASTE_OUTLINE, RGB565(0x574b67), 1);
					Glyphs::drawGlyphTx(x + 4, y + 18, PASTE_WHITE, RGB565(0xffffff), 1);
					Glyphs::drawGlyphTx(x + 4, y + 18, PASTE_SHADOW, RGB565(0xd3c0a8), 1);
				}
				break;
			case InvertButton:
				if (pressed) {
					Glyphs::drawGlyphTx(x + 5, y + 20, INVERT_OUTLINE, RGB565(0x574b67), 1);
					Glyphs::drawGlyphTx(x + 5, y + 20, INVERT_WHITE, RGB565(0xa39999), 1);
				} else {
					Glyphs::drawGlyphTx(x + 5, y + 19, INVERT_OUTLINE, RGB565(0x574b67), 1);
					Glyphs::drawGlyphTx(x + 5, y + 19, INVERT_WHITE, RGB565(0xffffff), 1);
				}
				break;
			case ClearButton:
				if (pressed) {
					Glyphs::drawGlyphTx(x + 4, y + 20, TRASH_OUTLINE, RGB565(0x574b67), 1);
					Glyphs::drawGlyphTx(x + 4, y + 20, TRASH_WHITE, RGB565(0xa39999), 1);
					Glyphs::drawGlyphTx(x + 4, y + 20, TRASH_SHADOW, RGB565(0xa39999), 1);
				} else {
					Glyphs::drawGlyphTx(x + 4, y + 19, TRASH_OUTLINE, RGB565(0x574b67), 1);
					Glyphs::drawGlyphTx(x + 4, y + 19, TRASH_WHITE, RGB565(0xffffff), 1);
					Glyphs::drawGlyphTx(x + 4, y + 19, TRASH_SHADOW, RGB565(0xd3c0a8), 1);
				}
				break;
			case DepthButton:
				if (pressed) {
					Glyphs::drawGlyphTx(x + 5, y + 20, LIGHTBULB_OUTLINE, RGB565(0x574b67), 1);
					Glyphs::drawGlyphTx(x + 5, y + 20, LIGHTBULB_WHITE, RGB565(0xa39999), 1);
					Glyphs::drawGlyphTx(x + 5, y + 20, LIGHTBULB_SHADOW, RGB565(0xa39999), 1);
				} else {
					Glyphs::drawGlyphTx(x + 5, y + 19, LIGHTBULB_OUTLINE, RGB565(0x574b67), 1);
					Glyphs::drawGlyphTx(x + 5, y + 19, LIGHTBULB_WHITE, RGB565(0xffffff), 1);
					Glyphs::drawGlyphTx(x + 5, y + 19, LIGHTBULB_SHADOW, RGB565(0xd3c0a8), 1);
				}
				break;
			case StopButton:
				if (pressed) {
					display->drawRectangleTx(x + 6, y + 7, 11, 11, RGB565(0x574b67));
					display->fillRectangleTx(x + 7, y + 8, 9, 9, RGB565(0x9c2e3b));
				} else {
					display->drawRectangleTx(x + 6, y + 6, 11, 11, RGB565(0x574b67));
					display->fillRectangleTx(x + 7, y + 7, 9, 9, RGB565(0xf04156));
				}
				break;
			case PlayButton:
				if (pressed) {
					Glyphs::drawGlyphTx(x + 5, y + 22, PLAY_OUTLINE, RGB565(0x574b67), 1);
					Glyphs::drawGlyphTx(x + 5, y + 22, PLAY_GREEN, RGB565(0x569138), 1);
				} else {
					Glyphs::drawGlyphTx(x + 5, y + 21, PLAY_OUTLINE, RGB565(0x574b67), 1);
					Glyphs::drawGlyphTx(x + 5, y + 21, PLAY_GREEN, RGB565(0x8ae25d), 1);
				}
				break;
			case CheckmarkButton:
				if (pressed) {
					Glyphs::drawGlyphTx(x + 4, y + 22, CHECKMARK_OUTLINE, RGB565(0x3b8367), 1);
					Glyphs::drawGlyphTx(x + 4, y + 22, CHECKMARK_WHITE, WHITE, 1);
				} else {
					Glyphs::drawGlyphTx(x + 4, y + 21, CHECKMARK_OUTLINE, RGB565(0x3b8367), 1);
					Glyphs::drawGlyphTx(x + 4, y + 21, CHECKMARK_WHITE, WHITE, 1);
				}
				break;
			default:
				ERROR(F("no id for roundbutton to render icon"));
				break;
		}
	}
	display->endWrite();
}