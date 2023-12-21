#include "ui/NavigationElement.h"

void NavigationElement::drawUnPressed() {
	display->startWrite();
	{
		// light purple bg
		display->fillRectangleTx(x + 1, y + 1, 46, 16, RGB565(0x937ac5));
		display->drawPixelTx(x + 45, y, RGB565(0x937ac5));

		// top-left highlight
		display->drawHorizontalLineTx(x + 1, y, 2, RGB565(0xf9f4ea));
		display->drawHorizontalLineTx(x, y + 1, 2, RGB565(0xf9f4ea));
		display->drawPixelTx(x, y + 2, RGB565(0xf9f4ea));

		// tan borders
		display->drawHorizontalLineTx(x + 3, y, 42, RGB565(0xd3c0a8));
		display->drawVerticalLineTx(x, y + 3, 13, RGB565(0xd3c0a8));

		// bottom and right dark purple shadows
		display->drawHorizontalLineTx(x, y + 19, 48, RGB565(0x2d1b2e));
		display->drawVerticalLineTx(x + 47, y, 20, RGB565(0x2d1b2e));
		display->drawPixelTx(x, y, RGB565(0x2d1b2e));
		display->drawPixelTx(x, y + 18, RGB565(0x2d1b2e));
		display->drawPixelTx(x + 46, y, RGB565(0x2d1b2e));
		display->drawPixelTx(x + 46, y + 18, RGB565(0x2d1b2e));

		// bottom purple shadow
		display->fillRectangleTx(x + 1, y + 17, 45, 2, RGB565(0x574b67));
		display->drawVerticalLineTx(x, y + 16, 2, RGB565(0x574b67));
		display->drawVerticalLineTx(x + 46, y + 16, 2, RGB565(0x574b67));

		if (direction == LeftNav) {
			Glyphs::drawGlyphTx(x + 17, y + 19, LEFT_ARROW_OUTLINE, RGB565(0x2d1b2e), 1);
			Glyphs::drawGlyphTx(x + 17, y + 19, LEFT_ARROW_WHITE, WHITE, 1);
		} else {
			Glyphs::drawGlyphTx(x + 18, y + 19, RIGHT_ARROW_OUTLINE, RGB565(0x2d1b2e), 1);
			Glyphs::drawGlyphTx(x + 18, y + 19, RIGHT_ARROW_WHITE, WHITE, 1);
		}
	}
	display->endWrite();
}

void NavigationElement::drawPressed() {
	display->startWrite();
	{
		// light purple bg
		display->fillRectangleTx(x, y + 1, 46, 17, RGB565(0x6547a0));

		// bottom-right highlight
		display->drawHorizontalLineTx(x + 1, y + 18, 45, RGB565(0xf9f4ea));
		display->drawVerticalLineTx(x + 46, y + 2, 16, RGB565(0xf9f4ea));
		display->drawPixelTx(x + 45, y + 17, RGB565(0xf9f4ea));

		// outer dark purple
		display->drawHorizontalLineTx(x, y, 48, RGB565(0x2d1b2e));
		display->drawHorizontalLineTx(x, y + 19, 48, RGB565(0x2d1b2e));
		display->drawPixelTx(x, y + 1, RGB565(0x2d1b2e));
		display->drawPixelTx(x, y + 18, RGB565(0x2d1b2e));
		display->drawPixelTx(x + 46, y + 18, RGB565(0x2d1b2e));
		display->drawPixelTx(x + 46, y + 1, RGB565(0x2d1b2e));

		if (direction == LeftNav) {
			Glyphs::drawGlyphTx(x + 18, y + 20, LEFT_ARROW_OUTLINE, RGB565(0x2d1b2e), 1);
			Glyphs::drawGlyphTx(x + 18, y + 20, LEFT_ARROW_WHITE, WHITE, 1);
		} else {
			Glyphs::drawGlyphTx(x + 19, y + 20, RIGHT_ARROW_OUTLINE, RGB565(0x2d1b2e), 1);
			Glyphs::drawGlyphTx(x + 19, y + 20, RIGHT_ARROW_WHITE, WHITE, 1);
		}
	}
	display->endWrite();
}



void WordTabElement::render() {
	TabElement::render();

	Text* text;
	switch (type) {
		case VisualTab:
			text = new Text("Visual");
			break;
		case CollisionTab:
			text = new Text("Collision");
			break;
		case AdvancedTab:
			text = new Text("Advanced");
			break;
		case ProjectSettingsTab:
			text = new Text("Project");
			break;
		case SystemSettingsTab:
			text = new Text(F("System"));
			break;
	}
	const uint16_t padding = text->getPaddingToCenter(width);
	if (selected) {
		text->setPosition(x + padding, y + 13);
		text->setStyle(1, RGB565(0xf9f4ea));
		text->setShadow(RGB565(0x574b67), true);
		text->print();
	} else {
		text->setPosition(x + padding, y + 12);
		text->setStyle(1, RGB565(0x937ac5));
		text->print();
	}

	delete text;
}

void TabElement::render() {
	display->startWrite();
	{
		if (selected) {
			// main light purple
			display->fillRectangleTx(x + 1, y + 0, width - 1, height - 6, RGB565(0x937ac5));

			// bottom grey purple
			display->fillRectangleTx(x + 1, y + height - 6, width - 1, 2, RGB565(0x574b67));
			display->drawPixelTx(x + 1, y + height - 7, RGB565(0x574b67));
			display->drawPixelTx(x + width - 1, y + 17, RGB565(0x574b67));

			// left most border
			display->drawVerticalLineTx(x + 0, y + 0, height - 4, RGB565(0x2d1b2e));

			// bottom border
			display->drawHorizontalLineTx(x + 1, y + 20, width - 1, RGB565(0x2d1b2e));
			display->drawPixelTx(x + 1, y + height - 5, RGB565(0x2d1b2e));
			display->drawPixelTx(x + width - 1, y + height - 5, RGB565(0x2d1b2e));
		} else {
			display->fillRectangleTx(x, y, width, 17, RGB565(0x2d1b2e)); // dark border

			display->fillRectangleTx(x, y + 7, width - 1, 8, RGB565(0x574b67)); // inner base color
			display->fillRectangleTx(x, y + 17, width, 7, RGB565(0x574b67)); // bottom base color
			display->drawPixelTx(x + width - 1, y + 16, RGB565(0x574b67)); // bottom right base color

			display->drawPixelTx(x, y + 14, RGB565(0x2d1b2e)); // inner left smooth
			display->drawPixelTx(x + width - 2, y + 14, RGB565(0x2d1b2e)); // inner right smooth
		}
	}
	display->endWrite();
}

void NumberTabElement::render() {
	TabElement::render();

	Text* text = new Text(number);
	const uint16_t padding = text->getPaddingToCenter(width);

	if (selected) {
		text->setPosition(x + padding + 1, y + 13);
		text->setStyle(1, RGB565(0xf9f4ea));
		text->setShadow(RGB565(0x574b67), true);
		text->print();
	} else {
		text->setPosition(x + padding, y + 12);
		text->setStyle(1, RGB565(0x937ac5));
		text->print();
	}

	delete text;
}