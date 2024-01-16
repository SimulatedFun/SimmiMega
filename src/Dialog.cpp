#include "Dialog.h"

// todo optimize and bring back ram versus eeprom
void Dialog::load(boolean fromRam) {
	if (dialogId >= dialogCount) {
		ERROR(F("dialog ") << dialogId << F(" OOB, cancelled load"));
		return;
	}
	const uint16_t address = dialogMemoryStart + (this->dialogId * dialogStructSize);
	for (uint8_t i = 0; i < dialogMaxLength; i++) {
		uint8_t buf = 0;
		eeprom->read(&buf, address + i, 1);
		buffer[i] = buf;
		RAW(buf << " ");
		if (buf == GLYPH_END_OF_LINE) {
			break;
		}
	}
	RAW("\n");
	calculateLength();
	boxCount = getNumBoxes();
}

void Dialog::save() {
	if (dialogId >= dialogCount) {
		ERROR(F("dialog ") << dialogId << F(" OOB, cancelled save"));
		return;
	}
	const uint16_t address = dialogMemoryStart + (this->dialogId * dialogStructSize);
	for (uint8_t i = 0; i < dialogMaxLength; i++) {
		const uint8_t c = buffer[i];
		eeprom->write(c, address + i, 1);
		ram->write(c, address + i, 1);
		if (c == GLYPH_END_OF_LINE) {
			break;
		}
	}
}

void Dialog::toString() {
	RAW("dialog " << dialogId << ":\n");
	for (const uint8_t glyphId : buffer) {
		RAW(glyphId << ", ");
	}
	RAW("\n");
}

void Dialog::drawPreview(uint16_t xOff, uint16_t yOff, uint16_t width, uint8_t scale) {
	if (dialogId == 0) {
		DrawingUtils::simplePrint(xOff, yOff, F("(no dialog)"), RGB565(0x919191), scale);
		return;
	}

	if (length == 0) {
		DrawingUtils::simplePrint(xOff, yOff, F("(empty dialog)"), RGB565(0x919191), scale);
		return;
	}

	const uint8_t quoteGlyphId = Text::asciiToGlyphLookupId('\"');
	const uint8_t dotGlyphId = Text::asciiToGlyphLookupId('.');
	uint16_t color = WHITE;
	boolean continued = true;
	uint16_t cursorX = 0;
	display->startWrite();
	{
		cursorX += Glyphs::drawGlyphTx(cursorX + xOff, yOff, quoteGlyphId, color, scale);

		for (uint8_t glyphId : buffer) {
			if (glyphId == GLYPH_END_OF_LINE) {
				continued = false;
				break;
			}
			if (glyphId >= GLYPH_COLOR_1 and glyphId <= GLYPH_COLOR_15) {
				color = Keyboard::glyphToColor(glyphId);
				continue;
			}
			if (glyphId == GLYPH_NEW_LINE) {
				// replace with a space and handle as a space
				glyphId = Text::asciiToGlyphLookupId(' ');
			}
			cursorX += Glyphs::drawGlyphTx(cursorX + xOff, yOff, glyphId, color, scale);
			if (cursorX > width) {
				break;
			}
		}

		// draw ellipses if the dialog trails off
		color = WHITE;
		if (continued) {
			for (uint8_t i = 0; i < 3; i++) {
				cursorX += Glyphs::drawGlyphTx(cursorX + xOff, yOff, dotGlyphId, color, scale);
			}
		}

		cursorX += Glyphs::drawGlyphTx(cursorX + xOff, yOff, quoteGlyphId, color, scale);
	}
	display->endWrite();
}

uint8_t Dialog::getNumBoxes() {
	INFO("get num boxes");
	uint16_t x = 0;
	const uint8_t scale = 2;
	uint8_t currentPage = 1;
	uint8_t lineWithinBox = 0;

	for (const uint8_t c : buffer) {
		// new line
		if (c == GLYPH_NEW_LINE) {
			INFO("new line");
			x = 0;
			lineWithinBox++;
			if (lineWithinBox == 2) { // go on to the next dialog box
				lineWithinBox = 0;
				x = 0;
				currentPage++;
			}
			continue;
		}

		const uint8_t width = Glyphs::getGlyphPixelWidth(c, scale);
		// INFO("width of character " << c << " is " << width);
		//  if character exceeds width of box, go to next line
		if (x + width > dialogBoxInnerWidth * scale) {
			INFO("width exceeds the box width");
			x = 0;
			lineWithinBox++;
		}

		// go on to the next dialog box
		if (lineWithinBox == 2) {
			INFO("line is 2, thus page++");
			lineWithinBox = 0;
			x = 0;
			currentPage++;
		}

		if (c == GLYPH_END_OF_LINE) {
			if (currentPage > maxDialogPages) {
				ERROR(F("dialog has too many pages"));
				return maxDialogPages;
			}
			return currentPage;
		}

		x += width;
	}

	if (currentPage > maxDialogPages) {
		ERROR("dialog has too many pages");
		return maxDialogPages;
	}
	return currentPage;
}

void Dialog::calculateLength() {
	length = 0;
	for (const uint16_t c : buffer) {
		if (c == GLYPH_END_OF_LINE) {
			return;
		}
		length++;
	}
}

void Dialog::setCharacterAtIndex(uint8_t index, uint8_t glyphId) {
	if (index >= dialogMaxLength) {
		ERROR(F("set char index OOB: ") << index);
		return;
	}
	buffer[index] = glyphId;
	calculateLength(); // wouldn't need to re-calc if it's just a pop/push
}

void Dialog::clear() {
	length = 0;
	buffer[0] = GLYPH_END_OF_LINE;
}

boolean Dialog::isFull() {
	if (length < dialogMaxLength) {
		return false;
	}
	return true;
}
