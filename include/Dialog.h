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
		boolean isFull();
		boolean isEmpty() {
			return length == 0;
		}
		uint8_t getLength() {
			calculateLength();
			return length;
		}
		void setCharacterAtIndex(uint8_t index, uint8_t glyphId);
		void clear();
		void drawPreview(uint16_t xOff, uint16_t yOff, uint16_t width, uint8_t scale);
		uint8_t getNumBoxes();
		void toString();

	private:
		uint8_t length = 0;
		/// Calculates the number of characters in the dialog string
		void calculateLength();
};
constexpr uint8_t dialogTop = 0;
constexpr uint8_t dialogBottom = 1;
constexpr uint8_t dialogMiddle = 2;

constexpr uint8_t dialogBoxOuterHeight = 30;
constexpr uint8_t dialogBoxOuterWidth = 144;

#endif