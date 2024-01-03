#include "states/EditTextField.h"
#include "ui/TextFieldSelector.h"

/// Edits a text field such as the game title, description, or name of the directory in which the
/// game data is saved to. Used in conjunction with TextFieldSelector to show the preview text.
namespace EditTextField {
	RoundButton *exit, *checkmark; // either exit and save or exit and discard changes
	TextField* textArea;				 // used for easy drawing the text area
	Keyboard* keyboard;

	char* text; // title, description, and directory will only use ascii, no icons, etc.
	uint8_t maxLength = 0; // text's max length
	uint8_t textType = directoryMode; // type of text: title/description or directory name

	boolean callbackDoneEditing = false;
	boolean saveEdits = false;

	uint16_t index = 0; // current char in the array (cursor)

	/// Main function to call
	boolean editField(char* inputText, uint8_t length, uint8_t mode) {
		INFO(F("editField: ") << inputText << F(", len: ") << length << F(", mode: ") << mode);
		text = inputText;
		maxLength = length;
		textType = mode;

		setup();

		while (!callbackDoneEditing) {
			UIHelper::loop();
			loopFlashingCursor();
		}

		deallocate();
		return saveEdits;
	}

	void setup() {
		callbackDoneEditing = false;
		saveEdits = false;

		setupUI();
		touch->clearQueue(); // do before draw so you can touch faster
		refreshCursor();
		draw();
		resetTimer(DialogCursorFlash);
	}

	void setupUI() {
		UIHelper::clearActive();

		checkmark = new RoundButton(CheckmarkButton);
		checkmark->setPosition(280, 32);
		checkmark->callback.bind(&callbackFinishEditing);
		UIHelper::registerUI(checkmark);

		exit = new RoundButton(ExitButton);
		exit->setPosition(280, 56);
		exit->callback.bind(&callbackFinishEditing);
		UIHelper::registerUI(exit);

		textArea = new TextField();
		textArea->setPosition(32, 32);
		// no touch register, only for displaying text

		if (textType == directoryMode) { // todo handle other modes
			keyboard = new Keyboard(DirectoryMode);
		} else {
			keyboard = new Keyboard(NormalKeyboard);
		}
		keyboard->setPosition(0, 130);
		UIHelper::registerUI(keyboard);

		keyboard->key->callback.bind(&callbackKeyPressGlyph);
		keyboard->backspace->callback.bind(&callbackBackspace);
		keyboard->space->callback.bind(&callbackKeyPressGlyph);
		keyboard->registerKeys();
	}

	void deallocate() {
		UIHelper::clearActive();
		delete exit;
		delete checkmark;
		delete textArea;
		delete keyboard;
	}

	void draw() {
		DrawingUtils::dither(RGB565(0x323232), true);
		textArea->render();
		UIHelper::render();
		for (uint8_t i = 0; i < maxLength; i++) {
			if (text[i] == '\0') {
				break;
			}
			drawChar(i, false);
		}
		// 208 74 char counter
	}

	void callbackFinishEditing(RoundButton& btn) {
		btn.depressed = true;
		btn.render();
		callbackDoneEditing = true;
		if (&btn == checkmark) {
			saveEdits = true;
		} else {
			saveEdits = false;
		}
	}

	/// Count the number of glyphs before reaching a terminator
	void refreshCursor() {
		index = 0;
		for (uint8_t i = 0; i < maxLength; i++) {
			if (text[i] == '\0') {
				INFO(F("1index: ") << index);
				return;
			}
			index++;
		}
		INFO(F("2index: ") << index);
	}

	// Called in the main editor state loop
	void loopFlashingCursor() {
		if (!checkTimer(530, DialogCursorFlash, true)) {
			return;
		}

		uint16_t x = 0, y = 0;
		uint16_t color = WHITE;
		getCursor(&x, &y, &color, index);
		static boolean flashOn = true;
		if (flashOn) {
			color = WHITE;
		} else {
			color = RGB565(0x2d1b2e);
		}
		display->startWrite();
		display->drawVerticalLineTx(x, y - 10, 10, color);
		display->endWrite();
		flashOn = !flashOn;
		//INFO(F("loopFlashingCursor at (") << x << ", " << y << F("), index: ") << index);
	}

	void eraseCursor() {
		uint16_t x = 0;
		uint16_t y = 0;
		uint16_t color = WHITE;
		getCursor(&x, &y, &color, index);
		display->startWrite();
		display->drawVerticalLineTx(x, y - 10, 10, RGB565(0x2d1b2e));
		display->endWrite();
	}

	void callbackBackspace(uint8_t) {
		INFO(F("backspace pressed"));
		if (index == 0) {
			INFO(F("is empty"));
			return;
		}
		eraseCursor();
		drawChar(index - 1, true);
		text[index - 1] = '\0';
		index--;
	}

	void callbackKeyPressGlyph(uint8_t glyphId) {
		INFO(F("callbackKeyPressGlyph: ") << glyphId);

		if (index == maxLength) {
			INFO(F("nothing added, at max length"));
			return;
		}

		const char inChar = Text::glyphLookupIdToAscii(glyphId);

		if (index == 0) {
			// don't allow weird characters for the first char
			if (inChar == '-' or inChar == '_' or inChar == ' ') {
				return; // do nothing
			}
		}

		eraseCursor();
		text[index] = inChar;
		if (index < maxLength) {
			text[index + 1] = '\0';
		}
		drawChar(index, false);
		refreshCursor();
	}

	void drawChar(uint8_t charIndex, boolean erase) {
		uint16_t x = 0;
		uint16_t y = 0;
		uint16_t color = PINK_COLOR;
		getCursor(&x, &y, &color, charIndex);
		const uint8_t glyphId = Text::asciiToGlyphLookupId(text[charIndex]);
		display->startWrite();
		Glyphs::drawGlyphTx(x, y, glyphId, erase ? RGB565(0x2d1b2e) : color, 1);
		display->endWrite();
	}

	// get the x y of a letter in the text box
	// used for drawing the next glyph and for erasing the previous glyph
	const uint16_t topLeft_x = 40;
	const uint16_t topLeft_y = 49;
	void getCursor(uint16_t* x, uint16_t* y, uint16_t* color, uint8_t charIndex) {
		*color = WHITE;
		*x = 0;
		*y = 0;
		const uint8_t scale = 1;

		for (uint8_t i = 0; i < charIndex; i++) {
			if (text[i] == '\0') {
				return;
			}
			const uint8_t glyphId = Text::asciiToGlyphLookupId(text[i]);
			boolean addWidth = true;
			const uint8_t width = Glyphs::getGlyphPixelWidth(glyphId, scale);
			// if character exceeds width of box, go to next line
			if (*x + width > 220) {
				*x = 0;
				*y += 12 * scale;
				addWidth = false;
			}

			if (addWidth) {
				*x += width;
			}
		}

		*x += topLeft_x;
		*y += topLeft_y;
	}
} // namespace EditTextField