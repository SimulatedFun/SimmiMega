#ifndef Dropdowns_h
#define Dropdowns_h

#include <Arduino.h>
#include "ui/UIElement.h"
#include "ValueButton.h"
#include "LogicEditorElements.h"
#include "UIHelper.h"
#include "windows/ChooseFlag.h"
#include "states/LogicEditor.h"

class DropdownFlagUpdate : public UIElement {
	public:
		GameObject* obj;
		FlagSelector* touchFlag; // only one and we'll move it around for the dropdown
		TrueFalseSelection* trueFalse;
		FullScreen* outside;
		boolean dropdownOpen = false;

		void (*redrawFunction)(boolean) = nullptr;

		DropdownFlagUpdate() {
			width = 1; // unused but initial
			height = 1;
			outside = new FullScreen();
			touchFlag = new FlagSelector();
			trueFalse = new TrueFalseSelection();
		}
		~DropdownFlagUpdate() override {
			delete outside;
			delete touchFlag;
			delete trueFalse;
		}

		void handlePress() override {
			dropdownOpen = !dropdownOpen;

			// handle opening the dropdown
			if (dropdownOpen) {
				INFO(F("dropdown opened"));
				touch->clearQueue();
				UIHelper::clearActive();
				UIHelper::registerUI(this); // dropdown
				UIHelper::registerUI(outside);
				render();
				return;
			}

			const tsPoint_t pt = getRelativePoint();
			const uint8_t row = pt.y / 25;

			switch (row) {
				case 0: // no update to the flag
					obj->set_touchTriggersFlag(false);
					INFO(F("dropdown: no flag update"));
					break;
				case 1: // flips flag
					obj->set_touchTriggersFlag(true);
					obj->set_touchFlipsFlag(true);
					INFO(F("dropdown: flips flag"));
					break;
				case 2: // sets flag to some value
					obj->set_touchTriggersFlag(true);
					obj->set_touchFlipsFlag(false);
					INFO(F("dropdown: sets flag"));
					break;
				default:
					ERROR(F("touch OOB of dropdown"));
			}

			redrawFunction(true);
		}

		void render() override {
			// make sure the sub-elements are updated
			touchFlag->setData(obj->touchTriggersFlagId());
			trueFalse->state = obj->touchSetsFlagTo();

			if (dropdownOpen) {
				width = 156;
				height = 75;
				DrawingUtils::dither(RGB565(0x323232));
			} else {
				width = 156;
				height = 27;
			}
			UIElement::drawInnerBorder(false);
			UIElement::fillInner();
			renderArrowIcon(135, 12);
			if (dropdownOpen) {
				renderDashedLines();
				renderOpenDropdown();
			} else {
				renderClosedDropdown();
			}
		}

		void registerClosedDropdownUI() {
			// Register the sub-elements of the dropdown
			if (!obj->touchTriggersFlag()) {
				return;
			}
			if (!obj->touchFlipsFlag()) {
				UIHelper::registerUI(trueFalse);
			}
			UIHelper::registerUI(touchFlag);
		}
		void deregisterUI() {
			UIHelper::deregisterUI(outside);
			UIHelper::deregisterUI(trueFalse);
			UIHelper::deregisterUI(touchFlag);
			INFO(F("deregister flag update dropdown"));
		}
		/// Draws the contents above the dropdown button
		void renderClosedDropdown() {
			if (!obj->touchTriggersFlag()) {
				DrawingUtils::simplePrint(x + 8, y + 18, F("No Flag Update"), WHITE, 1);
				return;
			}

			if (obj->touchFlipsFlag()) {
				DrawingUtils::simplePrint(x + 8, y + 18, F("Flip"), WHITE, 1);
				touchFlag->setPosition(x + 29, y + 4);
				touchFlag->render();
				return;
			}

			DrawingUtils::simplePrint(x + 8, y + 18, F("Set"), WHITE, 1);
			touchFlag->setPosition(x + 28, y + 4);
			touchFlag->render();

			DrawingUtils::simplePrint(x + 73, y + 18, F("to"), WHITE, 1);
			trueFalse->setPosition(x + 89, y + 4);
			trueFalse->render();
		}

		void renderOpenDropdown() {
			DrawingUtils::simplePrint(x + 8, y + 18, F("No Flag Update"), WHITE, 1);

			DrawingUtils::simplePrint(x + 8, y + 42, F("Flip"), WHITE, 1);
			touchFlag->setPosition(x + 30, y + 29);
			touchFlag->render();

			DrawingUtils::simplePrint(x + 8, y + 66, F("Set"), WHITE, 1);
			touchFlag->setPosition(x + 29, y + 51);
			touchFlag->render();

			DrawingUtils::simplePrint(x + 73, y + 66, F("to"), WHITE, 1);
			trueFalse->setPosition(x + 89, y + 51);
			trueFalse->render();
		}

		void renderArrowIcon(uint16_t inX, uint16_t inY) {
			display->startWrite();
			{
				for (uint8_t i = 0; i < 6; i++) {
					const uint16_t xPos = x + inX + i;
					const uint16_t yPos = y + inY + i;
					const uint16_t lineWidth = 11 - (i * 2);
					display->drawHorizontalLineTx(xPos, yPos, lineWidth, RGB565(0x937ac5));
				}
			}
			display->endWrite();
		}

		void renderDashedLines() {
			// dashed lines separating options in dropdown
			display->startWrite();
			{
				const uint16_t color = RGB565(0x937ac5);
				for (uint16_t i = 0; i < width; i++) {
					if (i % 3 == 0) {
						display->drawPixelTx(x + 1 + i, y + 25, color);
						display->drawPixelTx(x + 1 + i, y + 49, color);
					}
				}
			}
			display->endWrite();
		}
};

#endif