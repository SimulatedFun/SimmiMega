#ifndef ValueButton_h
#define ValueButton_h

#include <cmath>

#include "ui/UIElement.h"
#include "globals/Bitwise.h"
#include "globals/DataBounds.h"
#include "globals/SerialDebugging.h"
#include "GameObject.h"
#include "DisplayExtra.h"
#include "Dialog.h"
#include "spi/MicroSD.h"
#include "FileManager.h"

/// ValueButton is any button in which content is previewed/displayed inside it
/// Examples include:
/// 1. frame selection buttons in the sprite editor
/// 2. player object in the room editor
/// 3. music file selected to play in the room

enum Frame: boolean { Frame1, Frame2 };

class SpriteFrameSelector : public UIElement {
	private:
		GameObject* obj = nullptr;
		Frame frame = Frame1;
		uint64_t* sprite = nullptr;

	public:
		boolean selected = false;

		SpriteFrameSelector() {
			width = 32;
			height = 32;
		}

		void setData(GameObject* inObj, Frame inFrame);
		void render() override;
		void drawPreviewPixel(uint8_t index);

		void handlePress() override {
			if (callback.function != nullptr) {
				callback.function(frame);
			}
		}

		Callback<Frame> callback;
};

/// Used to show the actively edited object in logic editor
/// also used to show the player object in the room editor
class ObjectSelector : public UIElement {
	private:
		GameObject* obj = nullptr;
		Palette* pal = nullptr;

	public:
		ObjectSelector() {
			width = 16;
			height = 16;
		}

		void setData(GameObject* inObj, Palette* inPal = nullptr) {
			obj = inObj;
			pal = inPal;
		}

		void render() override;

		void handlePress() override {
			if (callback.function != nullptr) {
				callback.function(true);
			}
		}

		Callback<boolean> callback;
};

class FlagSelector : public UIElement {
	private:
		uint8_t flagId = _NO_FLAG;

	public:
		Callback<boolean> callback;

		explicit FlagSelector() {
			width = 40;
			height = 19;
			zIndex = 1;
		}

		void setData(uint8_t inFlagId) {
			flagId = inFlagId;
		}

		void handlePress() override {
			if (callback.function != nullptr) {
				callback.function(true);
			}
		}

		void render() override {
			UIElement::drawInnerBorder(false);
			UIElement::fillInner();
			renderFlagIcon();
			renderFlagId();
		}

		void renderFlagIcon() {
			display->startWrite();
			Glyphs::drawGlyphTx(x + 5, y + 19, FLAG_ICON, WHITE, 1);
			display->endWrite();
		}

		void renderFlagId() {
			Text* text = new Text(flagId);
			text->setPosition(x + 15 + text->getPaddingToCenter(20), y + 14);
			text->setStyle(1, WHITE);
			text->print();
			delete text;
		}
};

class ObjectIdSelector : public UIElement {
	public:
		uint16_t gameObjectId = 0;
		boolean enabled = true;
		Callback<boolean> callback;

		explicit ObjectIdSelector() {
			width = 49;
			height = 24;
			zIndex = 1;
		}

		void handlePress() override {
			if (callback.function != nullptr) {
				callback.function(true);
			}
		}

		void render() override {
			renderBackground();
			renderInterior();
		}

		void renderBackground() {
			UIElement::fillInner();
			if (enabled) {
				UIElement::drawInnerBorder(false);
			} else {
				UIElement::drawInnerBorder(true);
			}
		}

		void renderInterior() {
			Palette* pal;
			if (enabled) {
				pal = new Palette(WHITE, DARK_GREY, BLACK);
			} else {
				// gray on gray to show disabled
				pal = new Palette(DARK_GREY, RGB565(0x2a2a2a), BLACK);
			}
			GameObject* obj = new GameObject(gameObjectId);
			obj->load();
			INFO(F("drawing inside selector the gameobject: ") << gameObjectId);
			drawGameObject(obj, pal, x + 4, y + 4, 2, first_frame, true);
			renderGameObjectId();
			delete pal;
			delete obj;
		}

		void renderGameObjectId() {
			Text* id = new Text(gameObjectId);
			uint16_t color;
			if (enabled) {
				color = WHITE;
			} else {
				color = DARK_GREY;
			}
			id->setPosition(x + 24 + id->getPaddingToCenter(20), y + 17);
			id->setStyle(1, color);
			id->print();
			delete id;
		}
};

/// Displays a bounded UI element containing an X and Y coordinate preview
class XYSelector : public UIElement {
	private:
		Coordinates coords;

	public:
		Callback<boolean> callback;

		explicit XYSelector() {
			width = 43;
			height = 19;
			zIndex = 1;
		}

		void setData(Coordinates inCoords) {
			coords = inCoords;
		}

		void handlePress() override {
			if (callback.function != nullptr) {
				callback.function(true);
			}
		}

		void render() override {
			UIElement::drawInnerBorder(false);
			renderCoords();
		}

		void renderCoords() {
			Text* text = new Text(coords);
			text->setPosition(x + 5 + text->getPaddingToCenter(34), y + 14);
			text->setStyle(1, WHITE);
			text->print();
			delete text;
		}
};

/// If SD inserted, show the name of the song file in the preview
/// If no SD, maybe display `01-XX.wav` or `song #01`
const uint8_t maxTracks = 36;

class MusicSelector : public UIElement {
	public:
		uint8_t trackId = _NO_MUSIC;
		Callback<boolean> callback;

		explicit MusicSelector() {
			width = 79;
			height = 16;
			zIndex = 1;
		}

		void handlePress() override {
			if (callback.function != nullptr) {
				callback.function(true);
			}
		}

		void render() override {
			UIElement::drawOuterBorder(true);
			renderTrackName();
		}

		void renderTrackName() {
			String trackName;

			microSd->begin();
			FileManager::getSongInfoTx(trackId, &trackName);
			microSd->end();

			Text* text;
			text = new Text(trackName);
			text->setStyle(1, WHITE);

			if (text->length < 5) {
				delete text;
				char buf[13] = "(track #    ";
				itoa(trackId, &buf[8], 10);
				if (trackId <= 9) {
					buf[9] = ')';
					buf[10] = '\0';
				} else {
					buf[10] = ')';
					buf[11] = '\0';
				}
				text = new Text(buf, 13);
				text->setStyle(1, RGB565(0x919191));
			}

			text->setPosition(x + text->getPaddingToCenter(width), y + 13);
			text->print();
			delete text;
		}
};

class RoomSelector : public UIElement {
	private:
		uint8_t roomId = _NO_ROOM;

	public:
		Callback<boolean> callback;

		explicit RoomSelector() {
			width = 43;
			height = 19;
			zIndex = 1;
		}

		void setData(uint8_t inRoomId) {
			roomId = inRoomId;
		}

		void handlePress() override {
			if (callback.function != nullptr) {
				callback.function(true);
			}
		}

		void render() override {
			UIElement::drawInnerBorder(false);
			renderRoomIcon();
			renderRoomId();
		}

		void renderRoomIcon() {
			display->startWrite();
			Glyphs::drawGlyphTx(x + 5, y + 19, DOOR_ICON, WHITE, 1);
			display->endWrite();
		}

		void renderRoomId() {
			Text* text = new Text(roomId);
			text->setPosition(x + 15 + text->getPaddingToCenter(20), y + 14);
			text->setStyle(1, WHITE);
			text->print();
			delete text;
		}
};

class DialogSelector : public UIElement {
	private:
		uint8_t dialogId = 0;

	public:
		Callback<boolean> callback;

		explicit DialogSelector() {
			width = 142;
			height = 19;
			zIndex = 1;
		}

		void setData(uint8_t inDialogId) {
			dialogId = inDialogId;
		}

		void handlePress() override {
			if (callback.function != nullptr) {
				callback.function(true);
			}
		}

		void render() override {
			UIElement::drawInnerBorder(false);
			UIElement::fillInner();
			renderPreview();
		}

		void renderPreview() {
			Dialog dialog(dialogId);
			dialog.load();
			uint16_t htPad = ((height - 9) / 2);
			if (height % 2 != 0) {
				htPad++;
			}
			dialog.drawPreview(x + 5, y + htPad + 9, width - 22, 1);
		}
};

class PaletteSelector : public UIElement {
	private:
		Palette* pal = nullptr;

	public:
		PaletteSelector() {
			width = 16;
			height = 16;
		}

		void setData(Palette* inPal) {
			pal = inPal;
		}

		void render() override;

		void handlePress() override {
			if (callback.function != nullptr) {
				callback.function(true);
			}
		}

		Callback<boolean> callback;
};

class PaletteDepthSelector : public UIElement {
	private:
		uint16_t color = PINK_COLOR;

	public:
		boolean selected = false;
		uint8_t depth = PALETTE_BACKGROUND;

		PaletteDepthSelector() {
			width = 32;
			height = 32;
		}

		void setData(uint16_t inColor) {
			color = inColor;
		}

		void render() override {
			UIElement::drawOuterBorder(true);
			renderColor();
		}

		void renderColor();

		void handlePress() override {
			selected = true;
			renderColor();
			if (callback.function != nullptr) {
				callback.function(depth);
			}
		}

		Callback<uint8_t> callback;
};

const uint8_t colorWheelTileSize = 10;
const uint8_t colorWheelRadius = 11;

// todo marching ants to show current selection on this UI element
class ColorWheel : public UIElement {
	private:
		/// Given an xy coord, returns the rgb565 value of the color picker circle
		/// \param x
		/// \param y
		/// \return rgb565 color of the circle at xy, considering darkness value
		uint16_t getCircleColor(int x, int y);

		/// Integer square root algorithm - included in production
		/// \param number calculates sqrt of number
		/// \return square root value rounded to nearest integer
		uint8_t int_sqrt(uint8_t number);

		/// Converts hsv to rgb
		/// \param rgb array of colors
		/// \param h hue
		/// \param s saturation
		void hsvtorgb(uint8_t* rgb, uint8_t h, uint8_t s);

	public:
		uint8_t darknessValue = 255;
		Callback<uint16_t> callback;

		ColorWheel() {
			width = 232;
			height = 232;
			debounceDelayMS = 0;
		}

		void render() override;
		void renderWheel();
		void handlePress() override;
};

// todo marching ants to show current selection on this UI element
class BrightnessSelector : public UIElement {
	public:
		uint8_t darknessValue = 255;
		Callback<uint8_t> callback;

		BrightnessSelector() {
			width = 16;
			height = 232;
		}

		void render() override;
		// void renderLoop();
		void handlePress() override;
};
#endif