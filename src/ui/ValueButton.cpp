#include "ui/ValueButton.h"

void SpriteFrameSelector::setData(GameObject* inObj, Frame inFrame) {
	obj = inObj;
	frame = inFrame;
	if (inFrame == Frame1) {
		sprite = &obj->data.frame1;
	} else {
		sprite = &obj->data.frame2;
	}
}

/// Initial draw function
void SpriteFrameSelector::render() {
	UIElement::drawOuterBorder(true);

	// draw the sprite inside
	for (uint8_t i = 0; i < 64; i++) {
		drawPreviewPixel(i);
	}
	if (selected) {
		UIElement::drawSelected(RED);
	}
}

/// Draws a single pixel in the frame preview (used while the user is editing
/// in the main sprite area
void SpriteFrameSelector::drawPreviewPixel(uint8_t index) {
	const boolean filled = testBit(*sprite, index);
	uint16_t color = BLACK;
	if (filled) {
		if (obj->isHighlighted()) {
			color = WHITE;
		} else {
			color = DARK_GREY;
		}
	}
	const uint16_t xPos = x + (index % 8) * 4;
	const uint16_t yPos = y + (index / 8) * 4;
	display->fillRectangle(xPos, yPos, 4, 4, color);

	// this part checks if we overdrew a pixel on the edge, because if so,
	// we need to redraw the selection dotted rectangle border
	if (xPos == 0 || yPos == 0 or yPos == 32 or xPos == 32) {
		UIElement::drawSelected(RED);
	}
}

void ObjectSelector::render() {
	UIElement::drawOuterBorder(true);
	if (pal == nullptr) {
		// use default palette
		Palette defaultPal(WHITE, DARK_GREY, BLACK);
		drawGameObject(obj, &defaultPal, x, y, 2, first_frame, true);
	} else {
		drawGameObject(obj, pal, x, y, 2, first_frame, true);
	}
}

void PaletteSelector::render() {
	UIElement::drawOuterBorder(true);
	display->startWrite();
	{
		display->fillRectangleTx(x, y, 16, 5, pal->getHighlight());
		display->fillRectangleTx(x, y + 5, 16, 6, pal->getForeground());
		display->fillRectangleTx(x, y + 11, 16, 5, pal->getBackground());
	}
	display->endWrite();
}

void PaletteDepthSelector::renderColor() {
	display->startWrite();
	{
		display->fillRectangleTx(x, y, width, height, color);
		if (selected) {
			display->drawDottedRectangleTx(x, y, width - 1, height - 1, RED);
		}
	}
	display->endWrite();
}

const float PI_FLOAT = 3.14159265F;
const float PIBY2_FLOAT = 1.5707963F;
/// Arc-tangent approximation function found online, slightly larger compile size than the built
/// in one ONLY used when calling storeAngles. Not compiled in production \param y y-coordinate
/// between 0 and 1 \param x x-coordinate between 0 and 1 \return returns angle
float atan2_approximation2(float y, float x) {
	if (x == 0.0F) {
		if (y > 0.0F) {
			return PIBY2_FLOAT;
		}
		if (y == 0.0F) {
			return 0.0F;
		}
		return -PIBY2_FLOAT;
	}
	float atan = NAN;
	const float z = y / x;
	if ((float) fabs(z) < 1.0F) {
		atan = z / (1.0F + 0.28F * z * z);
		if (x < 0.0F) {
			if (y < 0.0F) {
				return atan - PI_FLOAT;
			}
			return atan + PI_FLOAT;
		}
	} else {
		atan = PIBY2_FLOAT - z / (z * z + 0.28F);
		if (y < 0.0F) {
			return atan - PI_FLOAT;
		}
	}
	return atan;
}

// region ColorWheel Functions
uint16_t ColorWheel::getCircleColor(int x, int y) {
	const uint8_t distance = int_sqrt(x * x + y * y); // gets distance from (0, 0)
	if (distance > colorWheelRadius) {
		return BLACK; // color outside the radius as black
	}

	const double phi = atan2_approximation2(y, x);
	const double hue = ((phi + PI) / (2 * PI)) * 255;
	const uint8_t saturation = distance * ((255 * colorWheelTileSize) / ScreenHeight) * 2;

	uint8_t rgb[3];
	hsvtorgb(&rgb[0], hue, saturation);
	for (uint8_t& i : rgb) {
		i = (i * darknessValue) / 255;
	}
	return (uint16_t) (((rgb[0] & 0b11111000) << 8) | ((rgb[1] & 0b11111100) << 3) | (rgb[2] >> 3));
}

uint8_t ColorWheel::int_sqrt(uint8_t number) {
	if (number == 0) {
		return 0;
	}
	uint8_t s = 1;
	uint8_t t = number;
	while (s < t) {
		s <<= 1;
		t >>= 1;
	} // decide the value of the first tentative

	do {
		t = s;
		s = (number / s + s) >> 1; // x1=(N / x0 + x0)/2 : recurrence formula
	} while (s < t);

	return t;
}

void ColorWheel::hsvtorgb(uint8_t* rgb, uint8_t h, uint8_t s) {
	uint8_t temp[4];
	temp[3] = 255; // v

	// if color is grayscale
	if (s == 0) {
		rgb[0] = rgb[1] = rgb[2] = temp[3];
		return;
	}

	// make hue 0-5
	const uint8_t quadrant = h / 43;

	// find remainder part, make it from 0-255
	const uint8_t remainder = (h - (quadrant * 43)) * 6;

	// calculate temp vars, doing integer multiplication
	temp[0] = (temp[3] * (255 - s)) >> 8;
	temp[1] = (temp[3] * (255 - ((s * remainder) >> 8))) >> 8;
	temp[2] = (temp[3] * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

	const uint8_t data[6][3] = {{3, 2, 0}, {1, 3, 0}, {0, 3, 2}, {0, 1, 3}, {2, 0, 3}, {3, 0, 1}};

	for (uint8_t i = 0; i < 3; i++) {
		rgb[i] = temp[data[quadrant][i]];
	}
}

void ColorWheel::render() {
	UIElement::drawOuterBorder(true);
	renderWheel();
}

void ColorWheel::renderWheel() {
	for (int xPos = -colorWheelRadius; xPos <= colorWheelRadius; xPos++) {
		for (int yPos = -colorWheelRadius; yPos <= colorWheelRadius; yPos++) {
			const uint16_t temp_x = x + ((xPos + colorWheelRadius) * colorWheelTileSize);
			const uint16_t temp_y = y + ((yPos + colorWheelRadius) * colorWheelTileSize);
			display->fillRectangle(temp_x, temp_y, colorWheelTileSize, colorWheelTileSize,
										  getCircleColor(xPos, yPos));
		}
	}
}

void ColorWheel::handlePress() {
	const int touchX = (int) (getRelativeX() / colorWheelTileSize - colorWheelRadius);
	const int touchY = (int) (getRelativeY() / colorWheelTileSize - colorWheelRadius);
	const uint16_t color = getCircleColor(touchX, touchY);
	if (callback.function != nullptr) {
		callback.function(color);
	}
}
// endregion

const uint8_t totalSteps = 255;
const uint8_t blockHeight = 4;
void BrightnessSelector::render() {
	UIElement::drawOuterBorder(true);
	display->startWrite();
	{
		for (uint16_t yPos = 0; yPos < height; yPos += blockHeight) {
			const uint8_t colorStep = 255 - (yPos * totalSteps / height);
			const uint16_t color = RGB565(colorStep, colorStep, colorStep);
			display->fillRectangleTx(x, y + yPos, width, blockHeight, color);
		}
	}
	display->endWrite();
}
//
//void BrightnessSelector::renderLoop() {
//	// draw marching ants
//	const uint8_t yPos = darknessValue * blockHeight;
//}

void BrightnessSelector::handlePress() {
	// this maps y values from 240 to 255 linearly
	const uint8_t newDarkness = 255 - (getRelativeY() + (getRelativeY() / 6));
	if (darknessValue == newDarkness) {
		return;
	}
	darknessValue = newDarkness;
	if (callback.function != nullptr) {
		callback.function(newDarkness);
	}
}