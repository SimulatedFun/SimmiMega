#include "ui/UIHelper.h"

UIElement* activeElements[maxActiveUiElements] = {nullptr};

void UIHelper::clearActive() {
	for (auto& e : activeElements) {
		e = nullptr;
	}
}

void UIHelper::registerUI(UIElement* ui) {
	if (ui->width == 0 or ui->height == 0) {
		ERROR(F("critical error, UI element with no bounding area"));
	}
	for (auto& e : activeElements) {
		if (e == nullptr) {
			e = ui;
			return;
		}
	}
	ERROR(F("no free ui slots!"));
}

void UIHelper::deregisterUI(UIElement* ui) {
	for (auto& e : activeElements) {
		if (e == ui) {
			e = nullptr;
			return;
		}
	}
	ERROR(F("no ui to de-register"));
}

/// Important function to include in any loop
/// Polls the touchscreen to check if the user is touching the screen
/// Handles firing off touch events to any active UIElements
void UIHelper::loop() {
	touch->poll();
	// if touch not pressed, un-press all ui elements
	if (!touch->isPressed()) {
		for (auto& e : activeElements) {
			if (e == nullptr) {
				continue;
			}
			e->isPressed = e->oldPressed = false; // un-press
		}
		return;
	}

	UIElement* highestZ = nullptr;

	for (auto& e : activeElements) {
		if (e == nullptr) {
			continue;
		}
		// If within width/height bounds
		e->isPressed = isWithinTouchBounds(e);

		if (e->isPressed) {
			// Brand-new press (going from unpressed to pressed)
			// exception made for 0 delay buttons such as drawing areas "fast trigger"
			if (wasNewlyPressed(e) or e->debounceDelayMS == 0) {
				if (checkTimer(e->debounceDelayMS, BetweenTouches, false)) {
					if (highestZ == nullptr) {
						highestZ = e;
					} else {
						if (e->zIndex > highestZ->zIndex) {
							highestZ = e;
						}
					}
				}
			}
		}
	}

	if (highestZ != nullptr) {
		highestZ->oldPressed = highestZ->isPressed;
		resetTimer(BetweenTouches);
		resetTimer(ScreenChangeWait);
		highestZ->handlePress();
	}
	// fire off handlePress() of a single UIElement with the highest Z index
}

/// Renders all active UIElements
void UIHelper::render() {
	for (UIElement* b : activeElements) {
		if (b != nullptr) {
			b->render();
		}
	}
}

/// Brand new press event (going from unpressed to pressed)
boolean UIHelper::wasNewlyPressed(UIElement* e) {
	return (e->isPressed and !e->oldPressed);
}

/// If the last touch press landed within the bounds of this UIElement
boolean UIHelper::isWithinTouchBounds(UIElement* e) {
	const boolean inBounds = (touch->touchX() > e->x and touch->touchX() < (e->x + e->width) and
									  touch->touchY() > e->y and touch->touchY() < (e->y + e->height));
	//		if (inBounds) {
	//			DEBUG("touch (" << touch->touchX() << ", " << touch->touchY() << ") "
	//								 << F("is within UIElement bounds: (") << e->x << ", " << e->y
	//								 << F(") width: ") << e->width << F(", height: ") << e->height);
	//		}
	return inBounds;
}