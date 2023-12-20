#include "ui/UIElement.h"

UIElement::~UIElement() = default;

void UIElement::drawOuterBorder(boolean glint) const {
	display->startWrite();
	{
		// dark purple borders
		display->fillRectangleTx(x - 4 + 1, y - 4 + 0, width + 6, 4, RGB565(0x2d1b2e));	// top
		display->fillRectangleTx(x - 4 + 1, y + height, width + 6, 4, RGB565(0x2d1b2e)); // bottom

		display->fillRectangleTx(x - 4 + 0, y - 4 + 1, 4, height + 6, RGB565(0x2d1b2e));
		display->fillRectangleTx(x - 4 + width + 4, y - 4 + 1, 4, height + 6, RGB565(0x2d1b2e));

		if (glint) {
			display->fillRectangleTx(x - 4 + 2, y - 4 + 1, 2, 1, RGB565(0xffefcc)); // white glint
			display->fillRectangleTx(x - 4 + 1, y - 4 + 2, 1, 2, RGB565(0xffefcc));
			display->drawPixelTx(x - 4 + 2, y - 4 + 2, RGB565(0xffefcc));
		}

		// lighter purple streak
		display->drawHorizontalLineTx(x, y - 4 + 1, width + 2, RGB565(0x9d7bc9)); // top
		display->drawHorizontalLineTx(x - 2, y + height + 2, width + 4, RGB565(0x9d7bc9)); // bottom
		display->drawVerticalLineTx(x - 3, y, height + 2, RGB565(0x9d7bc9)); // left
		display->drawVerticalLineTx(x + width + 2, y - 2, height + 4, RGB565(0x9d7bc9)); // right

		// corner light purple
		display->drawPixelTx(x - 4 + 2, y - 4 + height + 5, RGB565(0x9d7bc9));
		display->drawPixelTx(x - 4 + width + 5, y - 4 + 2, RGB565(0x9d7bc9));
		display->drawPixelTx(x - 4 + width + 5, y - 4 + height + 5, RGB565(0x9d7bc9));
	}
	display->endWrite();
}

void UIElement::fillInner() const {
	display->fillRectangle(x + 2, y + 2, width - 4, height - 4, RGB565(0x2d1b2e));
}

void UIElement::drawInnerBorder(boolean grey) const {
	uint16_t color = RGB565(0x937ac5);
	if (grey) {
		color = DARK_GREY;
	}
	display->startWrite();
	{
		// dark purple borders
		display->fillRectangleTx(x + 1, y, width - 2, 4, RGB565(0x2d1b2e));	// top
		display->fillRectangleTx(x + 1, y + height - 4, width - 2, 4, RGB565(0x2d1b2e)); // bottom
		display->fillRectangleTx(x, y + 1, 4, height - 2, RGB565(0x2d1b2e)); // left
		display->fillRectangleTx(x + width - 4, y + 1, 4, height - 2, RGB565(0x2d1b2e));

		// lighter purple streak
		display->drawHorizontalLineTx(x + 2, y + 1, width - 4, color); // top
		display->drawHorizontalLineTx(x + 2, y + height - 2, width - 4, color); // bottom
		display->drawVerticalLineTx(x + 1, y + 2, height - 4, color); // left
		display->drawVerticalLineTx(x + width - 2, y + 2, height - 4, color); // right

		// corner light purple
		display->drawPixelTx(x + 2, y + 2, color);
		display->drawPixelTx(x + 2, y + height - 3, color);
		display->drawPixelTx(x + width - 3, y + 2, color);
		display->drawPixelTx(x + width - 3, y + height - 3, color);
	}
	display->endWrite();
}

/// Draws a dashed bounding box centered on the edge of this UIElement
void UIElement::drawSelected(uint16_t color) const {
	display->startWrite();
	{
		display->drawDottedRectangleTx(x, y, width - 1, height - 1, color);
		display->drawDottedRectangleTx(x - 1, y - 1, width + 1, height + 1, color);
	}
	display->endWrite();
}
