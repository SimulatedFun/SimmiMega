#ifndef Keyboard_h
#define Keyboard_h

#include <Arduino.h>
#include "spi/Display.h"
#include "ui/UIHelper.h"
#include "Text.h"

using KeyType = uint8_t;
constexpr KeyType GlyphKey = 0;
constexpr KeyType BackspaceKey = 1;
constexpr KeyType EnterKey = 2;
constexpr KeyType SymbolsKey = 3;
constexpr KeyType ShiftKey = 4;
constexpr KeyType CapsKey = 5;
constexpr KeyType SpaceKey = 6;
constexpr KeyType ColorKey = 7;
class KeyboardKey : public UIElement {
	public:
		KeyType type;
		uint8_t glyphIdValue = 0; // glyphId set in the setKeyData
		boolean depressed = false;
		Callback<uint8_t> callback;
		explicit KeyboardKey(KeyType inType) : type(inType) {
			debounceDelayMS = 100;
			zIndex = 1;
			height = 22;
			switch (type) {
				case BackspaceKey:
					width = 44;
					debounceDelayMS = 1;
					break;
				case EnterKey:
					width = 56;
					break;
				case SymbolsKey:
					width = 54;
					break;
				case ShiftKey:
					width = 43;
					break;
				case CapsKey:
					width = 30;
					break;
				case SpaceKey:
					width = 125;
					break;
				case ColorKey:
				case GlyphKey:
				default:
					debounceDelayMS = 0; // controlled by Keyboard debounce
					width = 25;
					break;
			}
		}
		void render() override;
		void renderValue();
		void handlePress() override;
};
struct KeyPosition {
		uint16_t x;
		uint16_t y;
		uint8_t value;
		KeyType type;
		uint8_t shiftValue;
		KeyType shiftType;
		uint8_t secondaryValue;
		KeyType secondaryType;
};
enum KeyboardMode: uint8_t { NormalKeyboard, DirectoryMode, SymbolKeyboard };
class Keyboard : public UIElement {
	public:
		KeyboardKey *backspace, *enter, *caps, *shift, *symbols, *space;
		KeyboardKey* key;
		KeyboardMode mode = NormalKeyboard;
		Keyboard(KeyboardMode inMode) : mode(inMode) {
			debounceDelayMS = 75;
			width = 312;
			height = 110;
			zIndex = 0;
			key = new KeyboardKey(GlyphKey);
			backspace = new KeyboardKey(BackspaceKey);
			caps = new KeyboardKey(CapsKey);
			shift = new KeyboardKey(ShiftKey);
			space = new KeyboardKey(SpaceKey);
			if (mode != DirectoryMode) {
				symbols = new KeyboardKey(SymbolsKey);
				enter = new KeyboardKey(EnterKey);
			}
		}
		~Keyboard() {
			delete key;
			delete backspace;
			delete caps;
			delete shift;
			delete space;
			if (mode != DirectoryMode) {
				delete symbols;
				delete enter;
			}
		}
		void registerKeys() {
			// key is not registered, only checked for when pressing keyboard overlay

			backspace->setPosition(x + 268, y + 22);
			UIHelper::registerUI(backspace);

			caps->setPosition(x + 1, y + 44);
			UIHelper::registerUI(caps);

			shift->setPosition(x + 1, y + 66);
			UIHelper::registerUI(shift);

			space->setPosition(x + 105, y + 88);
			space->glyphIdValue = Text::asciiToGlyphLookupId(' ');
			UIHelper::registerUI(space);

			if (mode == NormalKeyboard or mode == SymbolKeyboard) {
				symbols->setPosition(x + 1, y + 88);
				UIHelper::registerUI(symbols);

				enter->setPosition(x + 256, y + 44);
				UIHelper::registerUI(enter);
			}
		}
		void render() override {
			renderKeys();
		}
		void renderKeys();
		void handlePress() override;
		void setKeyData(KeyPosition pos);

		static uint16_t glyphToColor(uint8_t glyphId);
};
#endif