#include "ui/SimpleButton.h"

void SimpleButton::render() {
	if (!depressed) {
		display->startWrite();
		{
			// inner light purple
			display->fillRectangleTx(x + 1, y + 1, width - 2, height - 4, color.mainColor);

			// top tan
			display->drawHorizontalLineTx(x + 1, y, width - 3, color.topColor);
			display->drawVerticalLineTx(x, y + 1, height - 5, color.topColor);

			// bottom shadow
			display->fillRectangleTx(x + 1, y + height - 3, width - 3, 2, color.shadowColor);
			display->drawVerticalLineTx(x + width - 2, y + height - 4, 2, color.shadowColor);
			display->drawVerticalLineTx(x, y + height - 4, 2, color.shadowColor);

			// outer dark purple
			display->drawVerticalLineTx(x + width - 1, y, height, color.bgColor);
			display->drawHorizontalLineTx(x, y + height - 1, width, color.bgColor);
			display->drawPixelTx(x, y, color.bgColor);
			display->drawPixelTx(x, y + height - 2, color.bgColor);
			display->drawPixelTx(x + width - 2, y, color.bgColor);
			display->drawPixelTx(x + width - 2, y + height - 2, color.bgColor);
		}
		display->endWrite();
	} else {
		display->startWrite();
		{
			// inner purple
			display->fillRectangleTx(x, y + 1, width - 2, height - 3, color.pressedMainColor);

			// top bg
			display->drawHorizontalLineTx(x + 1, y, width - 3, color.bgColor);

			// bottom tan
			display->drawHorizontalLineTx(x + 1, y + height - 2, width - 3, color.topColor);
			display->drawVerticalLineTx(x + width - 2, y + 2, height - 4, color.topColor);

			// top left
			display->drawPixelTx(x, y + 1, color.bgColor);
		}
		display->endWrite();
	}
	renderValue();
}

void SimpleButton::renderValue() {
	uint8_t offset = 0;
	if (depressed) {
		offset = 1;
	}

	if (text == nullptr) {
		return;
	}

	text->setPosition(x + text->getPaddingToCenter(width) + offset, y + 18 + offset);
	if (!depressed) {
		text->setStyle(1, WHITE);
		text->setShadow(RGB565(0x574b67), true);
	} else {
		text->setStyle(1, RGB565(0x7a7a7a));
	}
	text->print();
}

void SimpleButton::handlePress() {
	depressed = true;
	render();
	if (callback.function != nullptr) {
		callback.function(value);
	}
}