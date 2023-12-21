#ifndef LogicEditorElements_h
#define LogicEditorElements_h

#include <Arduino.h>
#include "ui/UIElement.h"
#include "DrawingUtils.h"

// Base class
class LogicEditorSelection : public UIElement {
	public:
		boolean enabled = true;
		void render() override {
			renderBackground();
			renderText();
		}
		void renderBackground() {
			UIElement::fillInner();
			if (enabled) {
				UIElement::drawInnerBorder(false);
			} else {
				UIElement::drawInnerBorder(true);
			}
		}
		virtual void renderText() = 0;
		void handlePress() override {
			if (callback.function != nullptr) {
				callback.function(true);
			}
		}
		Callback<boolean> callback;
};

/// Animated / Not Animated
class AnimatedSelection : public LogicEditorSelection {
	public:
		boolean animated;
		AnimatedSelection() {
			width = 130;
			height = 19;
		}
		void renderText() override;
		void handlePress() override {
			animated = !animated;
			renderText();
			if (callback.function != nullptr) {
				callback.function(animated);
			}
		}
};

/// 1 / 2
class OneTwoSelection : public LogicEditorSelection {
	public:
		boolean frame;
		OneTwoSelection() {
			width = 27;
			height = 19;
			zIndex = 1;
		}
		void renderText() override;
		void handlePress() override {
			frame = !frame;
			renderText();
			if (callback.function != nullptr) {
				callback.function(frame);
			}
		}
};

/// Target Player / Wander / Still
class MovementSelection : public LogicEditorSelection {
	public:
		MovementType type;
		Callback<MovementType> callback;
		MovementSelection() {
			width = 149;
			height = 19;
		}
		void renderText() override;
		void handlePress() override {
			if (type == 0) {
				type = 2;
			} else {
				type--;
			}
			renderText();
			if (callback.function != nullptr) {
				callback.function(type);
			}
		}
};

/// Solid / Passthrough
class SoliditySelection : public LogicEditorSelection {
	public:
		boolean solid = false;
		SoliditySelection() {
			width = 110;
			height = 19;
		}
		void setSolid(boolean inSolid) {
			solid = inSolid;
		}
		void renderText() override;
		void handlePress() override {
			solid = !solid;
			renderText();
			if (callback.function != nullptr) {
				callback.function(solid);
			}
		}
};

/// T / F
class TrueFalseSelection : public LogicEditorSelection {
	public:
		boolean state = false;
		TrueFalseSelection() {
			width = 30;
			height = 19;
			zIndex = 1;
		}
		void renderText() override;
		void handlePress() override {
			state = !state;
			renderText();
			if (callback.function != nullptr) {
				callback.function(state);
			}
		}
};

#endif