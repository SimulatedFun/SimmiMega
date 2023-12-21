#include "DisplayExtra.h"
#include "ui/Keyboard.h"
#include "states/Play.h"

void drawGameObjectAtCoords(GameObject* g, Palette* p, Coordinates* c, uint8_t scale, boolean frame, boolean fullDraw) {
	drawGameObject(g,p, (c->x * 24), (c->y * 24), scale, frame, fullDraw);
}

/// Draws a game object on the screen
/// \param obj GameObject loaded with frames and logic for the palette depth check
/// \param p Palette for color data
/// \param c Coordinates of where on the screen to draw (tiled, so multiple by 8 and the scale)
/// \param scale depending on where it's drawn (play mode = 3, room editor = 2, etc)
/// \param frame what frame of the gameobject to draw
/// \param fullDraw force all pixels to draw or only draw pixels that change between frames
void drawGameObject(GameObject* obj, Palette* p, uint16_t x, uint16_t y, uint8_t scale, boolean frame, boolean fullDraw) {
	if (!Play::isAnimated(obj) and !fullDraw) {
		//display->drawLine(x + 2, y + 2, x + 20, y + 20, RED);
		//display->drawLine(x + 2, y + 20, x + 20, y + 2, RED);
		return;
	}

	uint16_t filledColor;
	if (obj->isHighlighted()) {
		filledColor = p->getHighlight();
	} else {
		filledColor = p->getForeground();
	}

	// For each pixel of the GameObject's sprites (8x8)
	uint64_t* thisFrame;
	uint64_t* nextFrame;
	if (frame == first_frame) {
		thisFrame = &obj->data.frame1;
		nextFrame = &obj->data.frame2;
	} else {
		thisFrame = &obj->data.frame2;
		nextFrame = &obj->data.frame1;
	}

	display->startWrite();
	{
		for (int row = 0; row < 8; row++) {
			for (int col = 0; col < 8; col++) {
				const uint8_t index = row * 8 + col;
				const boolean isPixelFilled = testBit(*thisFrame, index);
				const boolean pixelChangesBetweenFrames = (testBit(*thisFrame, index) != testBit(*nextFrame, index));
				const uint16_t xPosition = x + col * scale;
				const uint16_t yPosition = y + row * scale;

				uint16_t color;
				if (isPixelFilled) {
					color = filledColor;
				} else {
					color = p->getBackground();
				}

				if (fullDraw) {
					display->fillRectangleTx(xPosition, yPosition, scale, scale, color);
					//display->drawPixelTx(xPosition, yPosition, ORANGE);
					continue;
				}

				// todo only upodate pixel if the object is animated in the current state though
				if (pixelChangesBetweenFrames) {
					display->fillRectangleTx(xPosition, yPosition, scale, scale, color);
					//display->drawPixelTx(xPosition, yPosition, GREEN);
					continue;
				}

				//display->drawPixelTx(xPosition, yPosition, BLACK);
			}
		}
	}
	display->endWrite();
}

void drawDialogBox(Dialog* dialog, uint8_t pageNumber, uint8_t topLeft_x, uint8_t topLeft_y, uint8_t scale) {
	INFO(F("drawDialogBox, id: ") << dialog->dialogId << F(" on page: ") << pageNumber);
	const uint16_t boxWidth = dialogBoxOuterWidth * scale;
	const uint16_t boxHeight = dialogBoxOuterHeight * scale;
	display->fillRectangle(topLeft_x, topLeft_y, boxWidth, boxHeight, BLACK);

	uint16_t x = 0;
	uint16_t y = 0;
	uint16_t color = WHITE;
	uint16_t currentPage = 0;
	uint8_t lineWithinBox = 0;

	for (const uint8_t c : dialog->buffer) {
		// new line
		if (c == GLYPH_NEW_LINE) {
			x = 0;
			y += 12 * scale;
			lineWithinBox++;
			if (lineWithinBox == 2) { // go on to the next dialog box
				lineWithinBox = 0;
				x = 0, y = 0;
				currentPage++;
			}
			continue;
		}

		if (c >= GLYPH_COLOR_1 and c <= GLYPH_COLOR_15) {
			color = Keyboard::glyphToColor(c);
			continue;
		}

		const uint8_t width = Glyphs::getGlyphPixelWidth(c, scale);
		// if character exceeds width of box, go to next line
		if (x + width > dialogBoxInnerWidth * scale) {
			x = 0;
			y += 12 * scale;
			lineWithinBox++;
		}

		// go on to the next dialog box
		if (lineWithinBox == 2) {
			lineWithinBox = 0;
			x = 0, y = 0;
			currentPage++;
		}

		if (currentPage == pageNumber) {
			// if end of string
			if (c == GLYPH_END_OF_LINE) {
				return;
			}

			display->startWrite();
			{
				const uint8_t glyph_x_offset = 6 * scale + topLeft_x;
				const uint8_t glyph_y_offset = 14 * scale + topLeft_y;
				Glyphs::drawGlyphTx(x + glyph_x_offset, y + glyph_y_offset, c, color, scale);
			}
			display->endWrite();
		}

		x += width;
	}
	INFO(F("reached end of full dialog (no end glyph)"));
}