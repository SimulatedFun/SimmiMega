#ifndef RoundButton_h
#define RoundButton_h

#include <Arduino.h>
#include "ui/UIElement.h"
#include "Callback.h"
#include "Glyphs.h"

// Base class for all rounded purple buttons
enum RoundButtonType {
	ExitButton,
	PencilButton,
	EraserButton,
	CopyButton,
	PasteButton,
	InvertButton,
	ClearButton,
	DepthButton,
	PlayButton,
	StopButton,
	CheckmarkButton
};

class RoundButton : public UIElement {
	public:
		explicit RoundButton(uint8_t inId) : identifier(inId) {
			width = 24;
			height = 24;
			debounceDelayMS = 200;
		}

		void drawIcon(boolean pressed);

		void render() override {
			if (depressed) {
				drawPressed();
				drawIcon(true);
			} else {
				drawUnPressed();
				drawIcon(false);
			}
		}

		uint8_t getIdentifier() const {
			return identifier;
		}

		void handlePress() override {
			if (callback.function != nullptr) {
				callback.function(*this);
			}
		}

		Callback<RoundButton&> callback;

	private:
		uint8_t identifier = 0;
		void drawUnPressed();
		void drawPressed();
};

#endif