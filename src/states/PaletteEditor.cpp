#include "states/PaletteEditor.h"

// Todo brightness should update the color selected

namespace PaletteEditor {
	Palette* palette;
	uint8_t darknessValue;
	uint8_t editColor;

	RoundButton* exit;
	PaletteDepthSelector *highlight, *foreground, *background;
	BrightnessSelector* brightness;
	ColorWheel* wheel;

	void setup() {
		INFO(F("enter palette editor"));

		const uint8_t paletteId = ChoosePalette::pick();
		if (paletteId == _NO_PALETTE) {
			state = MainMenuState;
			return;
		}

		palette = new Palette();
		palette->id = paletteId;
		palette->load();

		editColor = PALETTE_HIGHLIGHT;
		darknessValue = 255;

		setupUI();

		draw();
		touch->clearQueue();
	}

	void setupUI() {
		UIHelper::clearActive();

		exit = new RoundButton(ExitButton);
		exit->setPosition(288, 0);
		exit->callback.bind(&callbackExit);
		UIHelper::registerUI(exit);

		wheel = new ColorWheel();
		wheel->setPosition(4, 4);
		wheel->darknessValue = 255;
		wheel->callback.bind(&callbackSelectColor);
		UIHelper::registerUI(wheel);

		brightness = new BrightnessSelector();
		brightness->setPosition(248, 4);
		brightness->darknessValue = 255;
		brightness->callback.bind(&callbackSelectBrightness);
		UIHelper::registerUI(brightness);

		highlight = new PaletteDepthSelector();
		highlight->setPosition(276, 60);
		highlight->setData(palette->getHighlight());
		highlight->selected = true;
		highlight->depth = PALETTE_HIGHLIGHT;
		highlight->callback.bind(&callbackSelectDepth);
		UIHelper::registerUI(highlight);

		foreground = new PaletteDepthSelector();
		foreground->setPosition(276, 132);
		foreground->setData(palette->getForeground());
		foreground->selected = false;
		foreground->depth = PALETTE_FOREGROUND;
		foreground->callback.bind(&callbackSelectDepth);
		UIHelper::registerUI(foreground);

		background = new PaletteDepthSelector();
		background->setPosition(276, 204);
		background->setData(palette->getBackground());
		background->selected = false;
		background->depth = PALETTE_BACKGROUND;
		background->callback.bind(&callbackSelectDepth);
		UIHelper::registerUI(background);
	}

	void callbackExit(RoundButton&) {
		palette->save();
		deallocate();
		oldState = MainMenuState;
        state = PaletteEditorState; // go back to choose palette screen
	}

	void callbackSelectBrightness(uint8_t inBrightness) {
		INFO(F("selected brightness: ") << inBrightness);
		wheel->darknessValue = inBrightness;
		wheel->renderWheel();
	}

	/// Selecting a color from the color wheel
	void callbackSelectColor(uint16_t inColor) {
		INFO(F("callbackSelectColor: ") << inColor);
		palette->setColorByIndex(editColor, inColor);
		switch (editColor) {
			case PALETTE_HIGHLIGHT:
				highlight->setData(inColor);
				highlight->renderColor();
				break;
			case PALETTE_FOREGROUND:
				foreground->setData(inColor);
				foreground->renderColor();
				break;
			case PALETTE_BACKGROUND:
				background->setData(inColor);
				background->renderColor();
				break;
		}
	}

	/// Selecting what palette depth to set the color
	void callbackSelectDepth(uint8_t depth) {
		if (editColor == depth) {
			return; // no change
		}
		editColor = depth;
		switch (depth) {
			case PALETTE_BACKGROUND:
				highlight->selected = false;
				highlight->renderColor();
				foreground->selected = false;
				foreground->renderColor();
				break;
			case PALETTE_FOREGROUND:
				background->selected = false;
				background->renderColor();
				highlight->selected = false;
				highlight->renderColor();
				break;
			case PALETTE_HIGHLIGHT:
				background->selected = false;
				background->renderColor();
				foreground->selected = false;
				foreground->renderColor();
				break;
		}
	}

	void draw() {
		DrawingUtils::fill(RGB565(0x574b67));
		UIHelper::render();
		DrawingUtils::simplePrint(281, 52, F("light"), RGB565(0x9d7bc9), 1);
		DrawingUtils::simplePrint(284, 125, F("mid"), RGB565(0x9d7bc9), 1);
		DrawingUtils::simplePrint(281, 197, F("dark"), RGB565(0x9d7bc9), 1);
	}

	void loop() {
		UIHelper::loop();
	}

	void deallocate() {
		LEAK(F("color sel dealloc start"));
		delete exit;
		delete highlight;
		delete foreground;
		delete background;
		delete brightness;
		delete wheel;

		delete palette;
		LEAK(F("color sel dealloc end"));
	}
} // namespace PaletteEditor