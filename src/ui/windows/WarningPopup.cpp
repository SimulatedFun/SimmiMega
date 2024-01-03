#include "ui/windows/WarningPopup.h"

namespace WarningBox {
	boolean choiceMade = false;
	boolean choice = false;
	SimpleButton *confirmBtn, *cancelBtn;
	RoundButton* exit;

	void callbackMakeChoice(boolean inChoice) {
		choiceMade = true;
		choice = inChoice;
	}

	void callbackExit(RoundButton&) {
		callbackMakeChoice(false);
	}

	void setup() {
		UIHelper::clearActive();
		choiceMade = false;

		exit = new RoundButton(ExitButton);
		exit->setPosition(249, 32);
		exit->callback.bind(&callbackExit);
		UIHelper::registerUI(exit);
	}

	void drawWindow() {
		DrawingUtils::dither(RGB565(0x323232));
		display->startWrite();
		{
			const uint16_t x = 40;
			const uint16_t y = 31;
			const uint16_t width = 232;
			const uint16_t height = 176;
			// main bg
			display->fillRectangleTx(x + 4, y + 25, width - 6, 145, RGB565(0xeac592));

			// border
			display->fillRectangleTx(x + 2, y, width - 4, 2, RGB565(0x2d1b2e));
			display->fillRectangleTx(x + 2, y + height - 2, width - 4, 2, RGB565(0x2d1b2e));
			display->fillRectangleTx(x, y + 2, 2, height - 4, RGB565(0x2d1b2e));
			display->fillRectangleTx(x + width - 2, y + 2, 2, height - 4, RGB565(0x2d1b2e));

			// white inner border
			display->fillRectangleTx(x + 2, y + 4, 2, height - 12, WHITE);
			display->fillRectangleTx(x + 4, y + 2, 205, 2, WHITE);

			// shadow
			display->fillRectangleTx(x + 2, y + height - 6, width - 4, 4, RGB565(0xd88b76));
			display->fillRectangleTx(x + 2, y + height - 8, 2, 2, RGB565(0xd88b76));
			display->fillRectangleTx(x + width - 4, y + height - 8, 2, 2, RGB565(0xd88b76));

			// corner border
			display->fillRectangleTx(x + 2, y + 2, 2, 2, RGB565(0x2d1b2e));			 // top left
			display->fillRectangleTx(x + 2, y + height - 4, 2, 2, RGB565(0x2d1b2e)); // bottom left
			display->fillRectangleTx(x + width - 4, y + 2, 2, 2, RGB565(0x2d1b2e));	 // top right
			display->fillRectangleTx(x + width - 4, y + height - 4, 2, 2,
											 RGB565(0x2d1b2e)); // bottom right

			display->fillRectangleTx(x + 18, y + 40, 198, 80, RGB565(0xffe0b6));

			uint8_t swapCounter = 0; // swap colors every 8 pixels
			boolean swap = false;

			for (uint8_t yCaution = 0; yCaution < 21; yCaution++) {		// iterate down the y axis
				for (uint8_t xCaution = 0; xCaution < 205; xCaution++) { // iterate over the width

					display->drawPixelTx(x + xCaution + 4, y + yCaution + 4, swap ? YELLOW : BLACK);
					swapCounter++;
					if (swapCounter >= 8) {
						swapCounter = 0;
						swap = !swap;
					}
				}
				swapCounter++;
			}
		}
		display->endWrite();
	}

	void loop() {
		while (!choiceMade) {
			UIHelper::loop();
			Audio::buffer();
		}
	}

	bool choose(Text* confirm, Text* cancel, Text* msg, BtnColor confirmColor,
					BtnColor cancelColor) {
		LEAK(F("confirmation window open"));
		touch->clearQueue();

		setup();

		confirmBtn = new SimpleButton();
		confirmBtn->setPosition(62, 164); // 22 133
		confirmBtn->width = confirm->pixelWidth + 48;
		confirmBtn->color = confirmColor;
		confirmBtn->text = confirm;
		confirmBtn->value = true;
		confirmBtn->callback.bind(&callbackMakeChoice);
		UIHelper::registerUI(confirmBtn);

		cancelBtn = new SimpleButton();
		cancelBtn->width = cancel->pixelWidth + 48;
		cancelBtn->setPosition(270 - cancelBtn->width - 20, 164);
		cancelBtn->color = cancelColor;
		cancelBtn->text = cancel;
		cancelBtn->value = false;
		cancelBtn->callback.bind(&callbackMakeChoice);
		UIHelper::registerUI(cancelBtn);

		drawWindow();
		msg->setPosition(66, 88);
		msg->setStyle(1, BLACK);
		msg->print();
		UIHelper::render();

		loop();

		deallocate();
		delete confirmBtn;
		delete cancelBtn;

		INFO(F("confirmation window returns ") << choice);
		return choice;
	}

	bool showMessage(Text* msg, Text* btnText, BtnColor btnColor) {
		setup();

		confirmBtn = new SimpleButton();
		confirmBtn->setPosition(62, 164); // 22 133
		confirmBtn->width = btnText->pixelWidth + 48;
		confirmBtn->color = btnColor;
		confirmBtn->text = btnText;
		confirmBtn->value = true;
		confirmBtn->callback.bind(&callbackMakeChoice);
		UIHelper::registerUI(confirmBtn);

		drawWindow();
		msg->setPosition(66, 88);
		msg->setStyle(1, BLACK);
		msg->print();
		UIHelper::render();

		loop();
		deallocate();
		delete confirmBtn;
		return true;
	}

	void deallocate() {
		UIHelper::clearActive();
		delete exit;
		touch->clearQueue();
	}

	const uint16_t xStart = 67;
	const uint16_t yStart = 110;
	const uint16_t fullWidth = 181;
	const uint16_t height = 19;
	uint16_t stepPx = 0;
	uint16_t currentStep = 0;

	void setupProgressBar(uint16_t steps, uint16_t barBg) {
		currentStep = 0;
		stepPx = fullWidth / steps;

        // overdraw the text area
        display->fillRectangle(58, 71, 198, 80, RGB565(0xeac592));

        // draw new smaller text area for the save message
        display->fillRectangle(58, 84, 198, 54, RGB565(0xffe0b6));

		// progress bar outline
		display->drawRectangle(66, 109, 183, 21, RGB565(0x2d1b2e));

		// progress bar inner color
		display->fillRectangle(xStart, yStart, fullWidth, height, barBg);
	}

	void stepProgressBar(uint16_t barInner) {
		currentStep++;
		uint16_t currWidth = currentStep * stepPx;
		if (currWidth >= fullWidth) {
			currWidth = fullWidth;
		}
		display->fillRectangle(xStart, yStart, currWidth, height, barInner);
	}

	void finishProgressBar(uint16_t color) {
        // draw new smaller text area for the message
        display->fillRectangle(58, 84, 198, 54, RGB565(0xffe0b6));

        // progress bar outline
        display->drawRectangle(66, 109, 183, 21, RGB565(0x2d1b2e));

        // progress bar inner color
		display->fillRectangle(xStart, yStart, fullWidth, height, color);
    }
} // namespace WarningBox