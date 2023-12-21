#ifndef NavigationElement_h
#define NavigationElement_h

#include <Arduino.h>
#include "ui/UIElement.h"
#include "spi/Display.h"
#include "Glyphs.h"
class TabElement : public UIElement {
	public:
		boolean selected = false;
		void render();
};

class NumberTabElement : public TabElement {
	private:
		uint8_t number = 0;

	public:
		explicit NumberTabElement(uint8_t inNumber) : number(inNumber) {
			width = 24;
			height = 24;
		}
		void render() override;
		void handlePress() override {
			if (callback.function != nullptr) {
				callback.function(number);
			}
		}
		Callback<uint8_t> callback;
};

enum LogicTab {VisualTab, CollisionTab, AdvancedTab, ProjectSettingsTab, SystemSettingsTab};
class WordTabElement : public TabElement {
	private:
		LogicTab type;

	public:
		explicit WordTabElement(LogicTab inType) : type(inType) {
			width = 104;
			height = 24;
		}
		void render() override;
		void handlePress() override {
			if (callback.function != nullptr) {
				callback.function(type);
			}
		}
		Callback<LogicTab> callback;
};

/// The two navigation buttons (left and right) at the bottom of the tray nav component
enum NavDirection { RightNav, LeftNav };
class NavigationElement : public UIElement {
	private:
		NavDirection direction = RightNav;

	public:
		explicit NavigationElement(NavDirection d) : direction(d) {};
		~NavigationElement() override = default;

		void render() override {
			if (isPressed) {
				drawUnPressed();
			} else {
				drawPressed();
			}
		}
		void drawUnPressed();
		void drawPressed();
};

#endif