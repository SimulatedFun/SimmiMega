#include "states/DialogEditor.h"

// Todo symbols don't work yet
// Todo exit dialog to choose dialog instead of main menu

namespace DialogEditor {
	Dialog* dialog;
	Keyboard* keyboard;
	RoundButton *exit, *clear;

	uint8_t index = 0;

	/// Opens a dialog selection window, then enters the dialog editor
	void setup() {
		INFO(F("enter write dialog"));

		const uint8_t chosenDialogId = ChooseDialog::pick();
		if (chosenDialogId == _NO_DIALOG) {
			state = MainMenuState;
			return;
		}

		dialog = new Dialog();
		dialog->dialogId = chosenDialogId;
		dialog->load();
		dialog->toString();
		const uint8_t h = dialog->getNumBoxes();
		INFO(F("dialog->getNumBoxes = ") << h);

		setupUI();
		refreshCursor();
		touch->clearQueue();
		draw();
		resetTimer(DialogCursorFlash);
	}

	void setupUI() {
		LEAK(F("DialogEditor setupUI"));
		exit = new RoundButton(ExitButton);
		exit->setPosition(288, 0);
		exit->callback.bind(&callbackExit);
		UIHelper::registerUI(exit);

		clear = new RoundButton(ClearButton);
		clear->setPosition(288, 32);
		clear->callback.bind(&callbackClear);
		UIHelper::registerUI(clear);

		keyboard = new Keyboard(NormalKeyboard);
		keyboard->setPosition(0, 130);
		UIHelper::registerUI(keyboard);

		keyboard->key->callback.bind(&callbackKeyPressGlyph);
		keyboard->backspace->callback.bind(&callbackBackspace);
		keyboard->enter->callback.bind(&callbackNewline);
		keyboard->caps->callback.bind(&callbackCaps);
		keyboard->shift->callback.bind(&callbackShift);
		keyboard->symbols->callback.bind(&callbackSymbols);
		keyboard->space->callback.bind(&callbackKeyPressGlyph);
		keyboard->registerKeys();
	}

	void draw() {
		LEAK(F("DialogEditor draw"));
		DrawingUtils::fill(RGB565(0x574b67));
		UIHelper::render();
		drawDialogBoxes();
	}

	/// Frees memory used for the buttons and dialog character array
	void deallocate() {
		LEAK("write dialog dealloc start");
		delete keyboard;
		delete dialog;
		delete exit;
		delete clear;
		LEAK("write dialog dealloc end");
	}

	/// Looped from the main loop, checks for button presses
	void loop() {
		UIHelper::loop();
		loopFlashingCursor();
	}

	void drawChar(uint8_t charIndex, boolean erase) {
		uint16_t x = 0;
		uint16_t y = 0;
		uint16_t color = PINK_COLOR;
		getCursor(&x, &y, &color, charIndex);
		if (dialog->buffer[charIndex] >= _RESERVED_SPECIAL_GLYPHS) {
			return;
		}
		display->startWrite();
		{
			Glyphs::drawGlyphTx(x, y, dialog->buffer[charIndex], erase ? BLACK : color, 1);
		}
		display->endWrite();
	}

	// Called in the main editor state loop
	void loopFlashingCursor() {
		if (!checkTimer(530, DialogCursorFlash, true)) {
			return;
		}

		uint16_t x = 0;
		uint16_t y = 0;
		uint16_t color = WHITE;
		getCursor(&x, &y, &color, index);
		static boolean flashOn = true;
		display->startWrite();
		{
			if (flashOn) {
				display->drawVerticalLineTx(x, y - 10, 10, color);
			} else {
				display->drawVerticalLineTx(x, y - 10, 10, BLACK);
			}
		}
		display->endWrite();
		flashOn = !flashOn;
	}

	// When we delete or add a glyph, always redraw where the flashing cursor would be in case
	void eraseCursor() {
		uint16_t x = 0;
		uint16_t y = 0;
		uint16_t color = WHITE;
		getCursor(&x, &y, &color, index);
		display->startWrite();
		display->drawVerticalLineTx(x, y - 10, 10, BLACK);
		display->endWrite();
	}

	/// Count the number of glyphs before reaching a terminator
	void refreshCursor() {
		index = 0;
		for (const uint8_t glyphId : dialog->buffer) {
			if (glyphId == GLYPH_END_OF_LINE) {
				return;
			}
			index++;
		}
	}

	// get the x y of a letter in the dialog
	// used for drawing the next glyph and for erasing the previous glyph
	void getCursor(uint16_t* x, uint16_t* y, uint16_t* color, uint8_t charIndex) {
		*color = WHITE;
		*x = 0;
		*y = 0;
		uint16_t lineWithinBox = 0;
		uint16_t currentPage = 0;
		const uint8_t scale = 1;

		for (uint8_t i = 0; i < charIndex; i++) {
			const uint8_t c = dialog->buffer[i];
			if (c == GLYPH_NEW_LINE) {
				*x = 0;
				*y += 12 * scale;
				lineWithinBox++;
				if (lineWithinBox == 2) { // go on to the next dialog box
					lineWithinBox = 0;
					*x = 0, *y = 0;
					currentPage++;
				}
				continue;
			}

			if (c >= GLYPH_COLOR_1 and c <= GLYPH_COLOR_15) {
				*color = Keyboard::glyphToColor(c);
				continue;
			}

			boolean addWidth = true;
			const uint8_t width = Glyphs::getGlyphPixelWidth(c, scale);
			// if character exceeds width of box, go to next line
			if (*x + width > dialogBoxInnerWidth * scale) {
				*x = 0;
				*y += 12 * scale;
				lineWithinBox++;
				addWidth = false;
			}

			// go on to the next dialog box
			if (lineWithinBox == 2) {
				lineWithinBox = 0;
				*x = 0, *y = 0;
				currentPage++;
				addWidth = false;
			}

			if (addWidth) {
				*x += width;
			}
		}

		const uint16_t topLeft_x = (currentPage / 4) * (144 + 1);
		const uint16_t topLeft_y = (currentPage % 4) * (30 + 1);
		const uint8_t glyph_x_offset = 6 * scale + topLeft_x;
		const uint8_t glyph_y_offset = 14 * scale + topLeft_y;
		//INFO("topLeft_x " << topLeft_x << ", topLeft_y " << topLeft_y << ", " << glyph_x_offset
		//<< ", " << glyph_y_offset);
		//INFO("getCursor1: " << *x << ", " << *y);
		*x += glyph_x_offset;
		*y += glyph_y_offset;
		//INFO("getCursor2: " << *x << ", " << *y);
	}

	void callbackExit(RoundButton&) {
		LEAK(F("callback exit"));
		exit->depressed = true;
		exit->render();
		dialog->save();
		state = MainMenuState;
		deallocate();
	}

	void callbackClear(RoundButton&) {
		clear->depressed = true;
		clear->render();

		eraseCursor();
		// we iterate backwards, so we can draw as we erase characters
		for (uint8_t i = dialog->getLength(); i > 0; i--) {
			drawChar(i, true);
			dialog->setCharacterAtIndex(i, GLYPH_END_OF_LINE);
		}
		drawChar(0, true);
		dialog->setCharacterAtIndex(0, GLYPH_END_OF_LINE);

		clear->depressed = false;
		clear->render();
		index = 0;
	}

	void callbackBackspace(uint8_t) {
		INFO(F("backspace pressed"));
		if (dialog->isEmpty()) {
			INFO(F("dialog is empty"));
			return;
		}
		eraseCursor();
		drawChar(index - 1, true);
		dialog->setCharacterAtIndex(index - 1, GLYPH_END_OF_LINE);
		index--;
	}

	void callbackNewline(uint8_t) {
		INFO(F("newline pressed"));
		eraseCursor();
		dialog->setCharacterAtIndex(index, GLYPH_NEW_LINE);
		if (index < dialogMaxLength - 1) {
			dialog->setCharacterAtIndex(index + 1, GLYPH_END_OF_LINE);
		}
		index++;
	}

	void callbackSymbols(uint8_t) {
		INFO(F("symbols pressed"));
		if (keyboard->mode == NormalKeyboard) {
			keyboard->mode = SymbolKeyboard;
		} else {
			keyboard->mode = NormalKeyboard;
		}
		keyboard->render();
	}

	void callbackShift(uint8_t) {
		INFO(F("shift press callback"));
		keyboard->render();
	}

	void callbackCaps(uint8_t) {
		INFO(F("caps press callback"));
		keyboard->render();
	}

	// TODO how to handle colors and glyphs coming in?
	void callbackKeyPressGlyph(uint8_t inGlyphId) {
		INFO(F("callbackKeyPressGlyph: ") << inGlyphId);

		if (dialog->isFull()) {
			INFO(F("nothing added, dialog at max length"));
			return;
		}

		eraseCursor();

		if (inGlyphId >= GLYPH_COLOR_1 and inGlyphId <= GLYPH_COLOR_15) {
			dialog->setCharacterAtIndex(index, inGlyphId);
			if (index < dialogMaxLength - 1) {
				dialog->setCharacterAtIndex(index + 1, GLYPH_END_OF_LINE);
			}
			drawChar(index, false);
			refreshCursor();
			return;
		}

		dialog->setCharacterAtIndex(index, inGlyphId);
		if (index < dialogMaxLength - 1) {
			dialog->setCharacterAtIndex(index + 1, GLYPH_END_OF_LINE);
		}
		drawChar(index, false);
		refreshCursor();
	}

	void drawDialogBoxes() {
		for (uint8_t page = 0; page < maxDialogPages; page++) {
			const uint16_t xPos = 1 + (page / 4) * (144);
			const uint16_t yPos = 1 + (page % 4) * (30 + 1);
			display->fillRectangle(xPos, yPos, 143, 30, BLACK);
		}
		for (uint8_t i = 0; i < dialog->getLength(); i++) {
			drawChar(i, false);
		}
	}
	// endregion
} // namespace DialogEditor