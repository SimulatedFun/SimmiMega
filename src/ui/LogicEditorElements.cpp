#include "ui/LogicEditorElements.h"

const uint16_t selectedColor = WHITE;
const uint16_t notSelectedColor = RGB565(0x6b6b6b);
const uint16_t dividerColor = RGB565(0x1f8dd2);
const uint16_t disabledColor = RGB565(0x6b6b6b);

void AnimatedSelection::renderText() {
	uint16_t animatedColor = disabledColor;
	uint16_t notAnimatedColor = disabledColor;
	uint16_t slashColor = disabledColor;
	if (enabled) {
		slashColor = dividerColor;
		if (animated) {
			animatedColor = selectedColor;
			notAnimatedColor = notSelectedColor;
		} else {
			notAnimatedColor = selectedColor;
			animatedColor = notSelectedColor;
		}
	}
	DrawingUtils::simplePrint(x + 5, y + 14, F("Animated"), animatedColor, 1);
	DrawingUtils::simplePrint(x + 53, y + 14, F("/"), slashColor, 1);
	DrawingUtils::simplePrint(x + 60, y + 14, F("Not Animated"), notAnimatedColor, 1);
}

void OneTwoSelection::renderText() {
	uint16_t oneColor = disabledColor;
	uint16_t twoColor = disabledColor;
	uint16_t slashColor = disabledColor;
	if (enabled) {
		slashColor = dividerColor;
		if (frame == first_frame) {
			oneColor = selectedColor;
			twoColor = notSelectedColor;
		} else {
			twoColor = selectedColor;
			oneColor = notSelectedColor;
		}
	}
	DrawingUtils::simplePrint(x + 5, y + 14, F("1"), oneColor, 1);
	DrawingUtils::simplePrint(x + 11, y + 14, F("/"), slashColor, 1);
	DrawingUtils::simplePrint(x + 17, y + 14, F("2"), twoColor, 1);
}

void MovementSelection::renderText() {
	uint16_t targetPlayerColor = disabledColor;
	uint16_t wanderColor = disabledColor;
	uint16_t stillColor = disabledColor;
	uint16_t slashColor = disabledColor;

	if (enabled) {
		slashColor = dividerColor;
		switch (type) {
			case movementTarget:
				targetPlayerColor = selectedColor;
				break;
			case movementWander:
				wanderColor = selectedColor;
				break;
			case movementStill:
				stillColor = selectedColor;
				break;
		}
	}

	DrawingUtils::simplePrint(x + 5, y + 14, F("Target Player"), targetPlayerColor, 1);
	DrawingUtils::simplePrint(x + 77, y + 14, F("/"), slashColor, 1);
	DrawingUtils::simplePrint(x + 83, y + 14, F("Wander"), wanderColor, 1);
	DrawingUtils::simplePrint(x + 120, y + 14, F("/"), slashColor, 1);
	DrawingUtils::simplePrint(x + 126, y + 14, F("Still"), stillColor, 1);
}

void SoliditySelection::renderText() {
	uint16_t solidColor = disabledColor;
	uint16_t passthroughColor = disabledColor;
	uint16_t slashColor = disabledColor;
	if (enabled) {
		slashColor = dividerColor;
		if (solid) {
			solidColor = selectedColor;
			passthroughColor = notSelectedColor;
		} else {
			passthroughColor = selectedColor;
			solidColor = notSelectedColor;
		}
	}
	DrawingUtils::simplePrint(x + 6, y + 14, F("Solid"), solidColor, 1);
	DrawingUtils::simplePrint(x + 33, y + 14, F("/"), slashColor, 1);
	DrawingUtils::simplePrint(x + 41, y + 14, F("Passthrough"), passthroughColor, 1);
}

void TrueFalseSelection::renderText() {
	uint16_t trueColor = disabledColor;
	uint16_t falseColor = disabledColor;
	uint16_t slashColor = disabledColor;
	if (enabled) {
		slashColor = dividerColor;
		if (state) {
			trueColor = selectedColor;
			falseColor = notSelectedColor;
		} else {
			falseColor = selectedColor;
			trueColor = notSelectedColor;
		}
	}
	DrawingUtils::simplePrint(x + 5, y + 14, F("T"), trueColor, 1);
	DrawingUtils::simplePrint(x + 14, y + 14, F("/"), slashColor, 1);
	DrawingUtils::simplePrint(x + 21, y + 14, F("F"), falseColor, 1);
}