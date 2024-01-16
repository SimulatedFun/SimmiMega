#ifndef DropdownTeleport_h
#define DropdownTeleport_h

#include <Arduino.h>
#include "ui/UIElement.h"
#include "ValueButton.h"
#include "LogicEditorElements.h"
#include "UIHelper.h"
#include "windows/ChooseFlag.h"
#include "states/LogicEditor.h"

class DropdownTeleport : public UIElement {
	public:
		GameObject* obj;
		boolean dropdownOpen = false;

		FullScreen* outside;
		FlagSelector* flagRequired;
		TrueFalseSelection* trueFalse;
		RoomSelector* roomSelector;
		XYSelector* xySelector;

		void (*redrawFunction)(boolean) = nullptr;

		// when open width/height = 273/99
		// when set to "only tp when ..." width/height = 273/51
		// when set to "always tp" or "no tp" width/height = 273/27
		DropdownTeleport() {
			width = 1; // unused but initial
			height = 1;
			outside = new FullScreen();
			flagRequired = new FlagSelector();
			trueFalse = new TrueFalseSelection();
			roomSelector = new RoomSelector();
			xySelector = new XYSelector();
		}

		~DropdownTeleport() override {
			delete outside;
			delete flagRequired;
			delete trueFalse;
			delete roomSelector;
			delete xySelector;
		}

		void handlePress() override {
			dropdownOpen = !dropdownOpen;

			// handle opening the dropdown
			if (dropdownOpen) {
				INFO("dropdown opened");
				touch->clearQueue();
				UIHelper::clearActive();
				UIHelper::registerUI(this); // dropdown
				UIHelper::registerUI(outside);
				render();
				return;
			}

			const tsPoint_t pt = getRelativePoint();

			// the dropdown is open, so let the user select one of the three items
			if (pt.y <= 25) {
				INFO(F("dropdown: no teleport"));
				obj->set_touchTeleportsToRoom(false);
			} else if (pt.y <= 50) {
				INFO(F("dropdown: always teleport"));
				obj->set_touchTeleportsToRoom(true);
				obj->set_touchTeleportRequiresFlag(false);
			} else if (pt.y <= 99) {
				INFO(F("dropdown: teleport on condition met"));
				obj->set_touchTeleportsToRoom(true);
				obj->set_touchTeleportRequiresFlag(true);
			}

			redrawFunction(true);
		}

		void render() override {
			// make sure the sub-elements are updated
			flagRequired->setData(obj->touchTeleportRequiresFlagId());
			trueFalse->state = obj->touchTeleportRequiresFlagSetTo();
			roomSelector->setData(obj->touchTeleportsToRoomId());
			const Coordinates coords(obj->touchTeleportsToRoomX(), obj->touchTeleportsToRoomY(), obj->touchTeleportsToRoomId());
			xySelector->setData(coords);

			if (dropdownOpen) {
				width = 262;
				height = 99;
				DrawingUtils::dither(RGB565(0x323232));
				UIElement::drawInnerBorder(false);
				UIElement::fillInner();
				renderArrowIcon(241, 12);
				renderDashedLines();
				renderOpenDropdown();
				return;
			}

			// dropdown closed
			if (obj->touchTeleportsToRoom() and obj->touchTeleportRequiresFlag()) {
				width = 262;
				height = 51;
				UIElement::drawInnerBorder(false);
				UIElement::fillInner();
				renderArrowIcon(241, 24);
			} else {
				// no teleport
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
			if (!obj->touchTeleportsToRoom()) {
				return;
			}

			// object gets teleported

			UIHelper::registerUI(roomSelector);
			UIHelper::registerUI(xySelector);

			// always teleported
			if (obj->touchTeleportRequiresFlag()) {
				UIHelper::registerUI(flagRequired);
				UIHelper::registerUI(trueFalse);
			}
		}
		void deregisterUI() {
			UIHelper::deregisterUI(outside);
			UIHelper::deregisterUI(flagRequired);
			UIHelper::deregisterUI(trueFalse);
			UIHelper::deregisterUI(roomSelector);
			UIHelper::deregisterUI(xySelector);
			INFO(F("deregister teleport dropdown"));
		}
		/// Draws the contents above the dropdown button
		void renderClosedDropdown() {
			if (!obj->touchTeleportsToRoom()) {
				DrawingUtils::simplePrint(x + 8, y + 18, F("No Teleportation"), WHITE, 1);
				return;
			}

			if (!obj->touchTeleportRequiresFlag()) {
				DrawingUtils::simplePrint(x + 8, y + 18, F("Always Teleport Player to"), WHITE, 1);
				roomSelector->setPosition(x + 137, y + 4);
				roomSelector->render();
				DrawingUtils::simplePrint(x + 183, y + 18, F("@"), WHITE, 1);
				xySelector->setPosition(x + 193, y + 4);
				xySelector->render();
				return;
			}

			DrawingUtils::simplePrint(x + 8, y + 18, F("Only Teleport Player to"), WHITE, 1);
			roomSelector->setPosition(x + 128, y + 4);
			roomSelector->render();

			DrawingUtils::simplePrint(x + 174, y + 18, F("@"), WHITE, 1);
			xySelector->setPosition(x + 184, y + 4);
			xySelector->render();

			DrawingUtils::simplePrint(x + 23, y + 42, F("when"), WHITE, 1);
			flagRequired->setPosition(x + 51, y + 28);
			flagRequired->render();

			DrawingUtils::simplePrint(x + 96, y + 42, F("is"), WHITE, 1);
			trueFalse->setPosition(x + 107, y + 28);
			trueFalse->render();

		}

		void renderOpenDropdown() {
			DrawingUtils::simplePrint(x + 8, y + 18, F("No Teleportation"), WHITE, 1);

			DrawingUtils::simplePrint(x + 8, y + 18 + 24, F("Always Teleport Player to"), WHITE, 1);
			roomSelector->setPosition(x + 137, y + 4 + 24);
			roomSelector->render();
			DrawingUtils::simplePrint(x + 183, y + 18 + 24, F("@"), WHITE, 1);
			xySelector->setPosition(x + 193, y + 4 + 24);
			xySelector->render();

			DrawingUtils::simplePrint(x + 8, y + 18 + 48, F("Only Teleport Player to"), WHITE, 1);
			roomSelector->setPosition(x + 128, y + 4 + 48);
			roomSelector->render();

			DrawingUtils::simplePrint(x + 174, y + 18 + 48, F("@"), WHITE, 1);
			xySelector->setPosition(x + 184, y + 4 + 48);
			xySelector->render();

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