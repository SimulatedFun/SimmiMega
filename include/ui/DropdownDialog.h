#ifndef DropdownDialog_h
#define DropdownDialog_h

#include <Arduino.h>
#include "ui/UIElement.h"
#include "ValueButton.h"
#include "LogicEditorElements.h"
#include "UIHelper.h"
#include "windows/ChooseFlag.h"
#include "states/LogicEditor.h"

class DropdownDialog : public UIElement {
	public:
		GameObject* obj;
		boolean dropdownOpen = false;

		FullScreen* outside;
		FlagSelector* flagRequired;
		TrueFalseSelection* trueFalse;
		DialogSelector* dialogSelector;

		void (*redrawFunction)(boolean) = nullptr;

		// when open width/height = 273/99
		// when set to "only tp when ..." width/height = 273/51
		// when set to "always tp" or "no tp" width/height = 273/27
		DropdownDialog() {
			width = 1; // unused but initial
			height = 1;
			outside = new FullScreen();
			flagRequired = new FlagSelector();
			trueFalse = new TrueFalseSelection();
			dialogSelector = new DialogSelector();
		}

		~DropdownDialog() override {
			delete outside;
			delete flagRequired;
			delete trueFalse;
			delete dialogSelector;
		}

		// does not handle inner ui elements)
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

			// the dropdown is open, so let the user select one of the three items
			if (getRelativeY() <= 25) {
				setPosition(x, 164);
				INFO(F("dropdown: no dialog"));
				obj->set_touchShowsDialog(false);
			} else if (getRelativeY() <= 50) {
				setPosition(x, 164);
				INFO(F("dropdown: always show dialog"));
				obj->set_touchShowsDialog(true);
				obj->set_touchShowsDialogRequiresFlag(false);
			} else if (getRelativeY() <= 99) {
				setPosition(x, 153);
				INFO(F("dropdown: show dialog on condition met"));
				obj->set_touchShowsDialog(true);
				obj->set_touchShowsDialogRequiresFlag(true);
			}

			redrawFunction(true);
		}

		void render() override {
			// make sure the sub-elements are updated
			flagRequired->setData(obj->dialogRequiresFlagId());
			trueFalse->state = obj->dialogRequiresFlagSetTo();
			dialogSelector->setData(obj->touchDialogId());

			if (dropdownOpen) {
				width = 262;
				height = 99;
				setPosition(x, 105);
				DrawingUtils::dither(RGB565(0x323232));
				UIElement::drawInnerBorder(false);
				UIElement::fillInner();
				renderArrowIcon(241, 12);
				renderDashedLines();
				renderOpenDropdown();
				return;
			}

			// show dialog when conditions met
			if (obj->touchShowsDialog() and obj->dialogRequiresFlag()) {
				width = 262;
				height = 51;
				UIElement::drawInnerBorder(false);
				UIElement::fillInner();
				renderArrowIcon(241, 24);
			} else {
				// no teleport or always
				width = 262;
				height = 27;
				UIElement::drawInnerBorder(false);
				UIElement::fillInner();
				renderArrowIcon(241, 12);
			}
			renderClosedDropdown();
		}


		void registerClosedDropdownUI() {
			// no teleportation
			if (!obj->touchShowsDialog()) {
				return;
			}

			UIHelper::registerUI(dialogSelector);

			if (obj->dialogRequiresFlag()) {
				UIHelper::registerUI(flagRequired);
				UIHelper::registerUI(trueFalse);
			}
		}

		void deregisterUI() {
			UIHelper::deregisterUI(outside);
			UIHelper::deregisterUI(flagRequired);
			UIHelper::deregisterUI(trueFalse);
			UIHelper::deregisterUI(dialogSelector);
			INFO(F("deregister dialog dropdown"));
		}
		/// Draws the contents above the dropdown button
		void renderClosedDropdown() {
			if (!obj->touchShowsDialog()) {
				DrawingUtils::simplePrint(x + 8, y + 18, F("No Dialog Display"), WHITE, 1);
				return;
			}

			if (!obj->dialogRequiresFlag()) {
				DrawingUtils::simplePrint(x + 8, y + 18, F("Always Show Dialog"), WHITE, 1);
				dialogSelector->setPosition(x + 106, y + 4);
				dialogSelector->render();
				return;
			}

			DrawingUtils::simplePrint(x + 8, y + 18, F("Only Show Dialog"), WHITE, 1);
			dialogSelector->setPosition(x + 95, y + 4);
			dialogSelector->render();

			DrawingUtils::simplePrint(x + 23, y + 42, F("when"), WHITE, 1);
			flagRequired->setPosition(x + 51, y + 28);
			flagRequired->render();

			DrawingUtils::simplePrint(x + 96, y + 42, F("is"), WHITE, 1);
			trueFalse->setPosition(x + 107, y + 28);
			trueFalse->render();
		}

		void renderOpenDropdown() {
			DrawingUtils::simplePrint(x + 8, y + 18, F("No Dialog Display"), WHITE, 1);

			DrawingUtils::simplePrint(x + 8, y + 18 + 24, F("Always Show Dialog"), WHITE, 1);
			dialogSelector->setPosition(x + 106, y + 4 + 24);
			dialogSelector->render();

			DrawingUtils::simplePrint(x + 8, y + 18 + 48, F("Only Show Dialog"), WHITE, 1);
			dialogSelector->setPosition(x + 95, y + 4 + 48);
			dialogSelector->render();

			DrawingUtils::simplePrint(x + 23, y + 90, F("when"), WHITE, 1);
			flagRequired->setPosition(x + 51, y + 76);
			flagRequired->render();

			DrawingUtils::simplePrint(x + 96, y + 90, F("is"), WHITE, 1);
			trueFalse->setPosition(x + 107, y + 76);
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
				for (uint16_t i = 0; i < width - 2; i++) {
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