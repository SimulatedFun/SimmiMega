#ifndef Dialog_h
#define Dialog_h

#include <Arduino.h>
#include "spi/Display.h"
#include "globals/DataBounds.h"
#include "Glyphs.h"
#include "RamBlock.h"
#include "spi/ExtEeprom.h"
#include "ui/Keyboard.h"

constexpr uint16_t dialogBoxInnerWidth = 132; // 288px - 24px (12px padding on ea side) / 2
constexpr uint8_t maxDialogPages = 8;
class Dialog {
	public:
		uint8_t dialogId = 0;
		uint8_t boxCount = 0;
		uint8_t buffer[dialogMaxLength];
		Dialog() {
			length = 0;
		}
		Dialog(uint8_t inDialogId) {
			dialogId = inDialogId;
		}
		void load(boolean fromRam = true);
		void save();
		/// Returns if the dialog is full or if more chars can be added
		boolean isFull() {
			if (length < dialogMaxLength) {
				return false;
			}
			return true;
		}
		boolean isEmpty() {
			return length == 0;
		}
		uint8_t getLength() {
			calculateLength();
			return length;
		}
		void setCharacterAtIndex(uint8_t index, uint8_t glyphId) {
			if (index >= dialogMaxLength) {
				ERROR(F("set char index OOB: ") << index);
				return;
			}
			buffer[index] = glyphId;
			// DEBUG(F("setCharacterAtIndex: ") << index << ", " << glyphId);
			calculateLength(); // wouldn't need to re-calc if it's just a pop/push
		}
		void clear() {
			length = 0;
			buffer[0] = GLYPH_END_OF_LINE;
		}
		void drawPreview(uint16_t xOff, uint16_t yOff, uint16_t width, uint8_t scale) {
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
		uint8_t getNumBoxes() {
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
		void toString();

	private:
		uint8_t length = 0;
		/// Calculates the number of characters in the dialog string
		void calculateLength() {
			length = 0;
			for (const uint16_t c : buffer) {
				if (c == GLYPH_END_OF_LINE) {
					return;
				}
				length++;
			}
		}
};
constexpr uint8_t dialogTop = 0;
constexpr uint8_t dialogBottom = 1;
constexpr uint8_t dialogMiddle = 2;

constexpr uint8_t dialogBoxOuterHeight = 30;
constexpr uint8_t dialogBoxOuterWidth = 144;

#endif