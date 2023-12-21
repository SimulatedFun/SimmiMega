/// Text can take almost any object or data and convert it to a readable string in order to
/// be printed out. Flash strings, coordinates, numbers, and regular char arrays. Text will
/// automatically generate the pixel width of this string, and implements the simple print()
/// function.
#ifndef Text_h
#define Text_h

#include <Arduino.h>
#include "spi/Display.h"
#include "GameData.h"
#include "Glyphs.h"

constexpr uint8_t textMaxLength = 200;
class Text {
	public:
		uint8_t length = 0;
		uint16_t pixelWidth = 0;
		explicit Text() = default;
		explicit Text(char c);
		explicit Text(const String& text);
		explicit Text(Coordinates c);
		explicit Text(uint16_t number);
		explicit Text(uint8_t number);
		explicit Text(const char* text, size_t len);
		explicit Text(char* text, size_t len);
		explicit Text(Title title) : Text (title.text.text, titleMaxLength) {}
		explicit Text(Description desc) : Text (desc.text, descriptionMaxLength) {}
		explicit Text(Folder folder) : Text (folder.text, directoryMaxLength) {}
		~Text();
		void setStyle(uint8_t scale, uint16_t color);
		void setShadow(uint16_t inShadowColor = PINK_COLOR, boolean inShowShadow = false);
		void setPosition(uint16_t inX, uint16_t inY);
		void calculatePixelWidth();
		uint16_t getPaddingToCenter(uint16_t availableWidth);
		void print();
		static char glyphLookupIdToAscii(uint8_t g);
		static uint8_t asciiToGlyphLookupId(char c);
		void setCharactersAtEnd(char* str, uint8_t len);

	private:
		char* characters = new char[textMaxLength];
		uint16_t x = 0, y = 0;
		uint8_t fontSize = 1;
		uint16_t color = WHITE;
		uint16_t shadowColor = RGB565(0x2d1b2e);
		boolean showShadow = false;
};

#endif