#include "Text.h"
Text::Text(char c) {
	length = 1;
	characters[0] = c;
	characters[1] = '\0';
	calculatePixelWidth();
}

Text::Text(Coordinates c) {
	char xBuf[4] = {'\0', '\0', '\0', '\0'};
	itoa(c.x, xBuf, 10);

	char yBuf[4] = {'\0', '\0', '\0', '\0'};
	itoa(c.y, yBuf, 10);

	uint8_t cursor = 0;
	characters[cursor++] = '(';
	cursor += strlcpy(characters + cursor, xBuf, textMaxLength);
	characters[cursor++] = ',';
	characters[cursor++] = ' ';
	cursor += strlcpy(characters + cursor, yBuf, textMaxLength);
	characters[cursor++] = ')';
	length = cursor;

	calculatePixelWidth();
}

Text::Text(uint16_t number) {
	char buf[7] = {'\0', '\0', '\0', '\0', '\0', '\0', '\0'};
	itoa(number, buf, 10);
	length = strlcpy(characters, buf, 6);
	calculatePixelWidth();
}

Text::Text(uint8_t number) {
	char buf[4] = {'\0', '\0', '\0', '\0'};
	itoa(number, buf, 10);
	length = strlcpy(characters, buf, 5);
	calculatePixelWidth();
}

Text::Text(const String& text) {
	length = strlcpy(characters, text.c_str(), text.length() + 1);
	calculatePixelWidth();
}

Text::Text(const char* text, size_t len) {
	length = strlcpy(characters, text, len + 1);
	calculatePixelWidth();
}

Text::Text(char* text, size_t len) {
	length = strlcpy(characters, text, len + 1);
	calculatePixelWidth();
}

Text::~Text() {
	delete[] characters;
}

void Text::setStyle(uint8_t scale, uint16_t inColor) {
	fontSize = scale;
	color = inColor;
	calculatePixelWidth();
}

void Text::setShadow(uint16_t inShadowColor, boolean inShowShadow) {
	shadowColor = inShadowColor;
	showShadow = inShowShadow;
}

void Text::setPosition(uint16_t inX, uint16_t inY) {
	x = inX;
	y = inY;
}

void Text::calculatePixelWidth() {
	pixelWidth = 0;
	for (uint8_t i = 0; i < length; i++) {
		if (characters[i] == '\0') {
			break;
		}
		const uint16_t glyphId = asciiToGlyphLookupId(characters[i]);
		pixelWidth += Glyphs::getGlyphPixelWidth(glyphId, fontSize);
	}
}

/// When we want to center a text object, we can pass this function in a width that we want
/// this text object to be centered in. Then this function will return an x-padding to offset
/// by, in order to appear centered.
uint16_t Text::getPaddingToCenter(uint16_t availableWidth) {
	uint16_t padding = (availableWidth / 2) - (pixelWidth / 2);
	if (availableWidth % 2 == 1 and pixelWidth % 2 == 1) {
		padding++;
	}
	return padding;
}

void Text::print() {
	uint16_t cursorX = x;
	uint16_t cursorY = y;
	display->startWrite();
	{
		for (uint8_t i = 0; i < length; i++) {
			// check for end of text
			if (characters[i] == '\0') {
				break;
			}

			// check for newline character
			if (characters[i] == '\n') {
				cursorX = x;
				cursorY += 14 * fontSize;
				continue;
			}

			// orders ascii in the same order as font
			const uint8_t glyphLookupId = asciiToGlyphLookupId(characters[i]);
			if (showShadow) {
				Glyphs::drawGlyphTx(cursorX + fontSize, cursorY + fontSize, glyphLookupId, shadowColor, fontSize);
			}
			cursorX += Glyphs::drawGlyphTx(cursorX, cursorY, glyphLookupId, color, fontSize);
		}
	}
	display->endWrite();
}

char Text::glyphLookupIdToAscii(uint8_t id) {
	if (id < 62) { // glyph lookup IDs 0-61 (a-z, A-Z, 0-9, and special characters)
		if (id < 26) {
			return 'a' + id; // 'a' = 0, 'b' = 1, etc.
		}
		if (id < 52) {
			return 'A' + (id - 26); // 'A' = 26, 'B' = 27, etc.
		}
		if (id < 62) {
			return '0' + (id - 52); // '0' = 52, '1' = 53, etc.
		}
	} else { // special cases beyond glyph lookup IDs 0-61
		switch (id) {
			case 62: return ' ';   // Space
			case 63: return '!';   // Exclamation mark
			case 64: return '?';   // Question mark
			case 65: return '/';   // Slash
			case 66: return '@';   // At symbol
			case 67: return '$';   // Dollar sign
			case 68: return '%';   // Percent sign
			case 69: return '&';   // Ampersand
			case 70: return '(';   // Opening parenthesis
			case 71: return ')';   // Closing parenthesis
			case 72: return '_';   // Underscore
			case 73: return '+';   // Plus sign
			case 74: return '-';   // Minus sign
			case 75: return '=';   // Equal sign
			case 76: return ',';   // Comma
			case 77: return '.';   // Period
			case 78: return ':';   // Colon
			case 79: return ';';   // Semicolon
			case 80: return '"';   // Double quote
			case 81: return '\'';  // Single quote
			case 82: return '#';   // Hash symbol
			case GLYPH_END_OF_LINE: return '\0';  // End of line
			case GLYPH_NEW_LINE: return '\n';      // New line
		}
	}
	// Default case (invalid glyph lookup ID)
	return '?';
}

/// Takes an input ascii character and converts it to a glyphLookupId for retrieving
/// the corresponding glyph/bitmap from the font file
uint8_t Text::asciiToGlyphLookupId(char c) {
	// nintendo font is a-z, A-Z, 0-9, then !?/@$%&()_+-=,.
	// in that exact order

	// glyph lookup IDs 0-25 (lower case letters)
	if (c >= 'a' and c <= 'z') {
		return c - 'a'; // making 'a' = 0, 'b' = 1, etc.
	}

	// glyph lookup IDs 26-51 (upper case letters)
	if (c >= 'A' and c <= 'Z') {
		return c - 'A' + 26;
	}

	// glyph lookup IDs 52-61 (digits)
	if (c >= '0' and c <= '9') {
		return c - '0' + 52;
	}

	// clang-format off
	switch (c) {
		case ' ': return 62;
		case '!': return 63;
		case '?': return 64;
		case '/': return 65;
		case '@': return 66;
		case '$': return 67;
		case '%': return 68;
		case '&': return 69;
		case '(': return 70;
		case ')': return 71;
		case '_': return 72;
		case '+': return 73;
		case '-': return 74;
		case '=': return 75;
		case ',': return 76;
		case '.': return 77;
		case ':': return 78;
		case ';': return 79;
		case '"': return 80;
		case '\'': return 81;
		case '#': return 82;
		case '\0': return GLYPH_END_OF_LINE;
		case '\n': return GLYPH_NEW_LINE;
		default:
			ERROR(F("asciiToGlyphLookupId no lookup for character: ") << c << ", uint: " << (uint8_t) c);
			return 64; // question mark
	}
	// clang-format on
}

void Text::setCharactersAtEnd(char* str, uint8_t len) {
	if (length + len >= textMaxLength) {
		ERROR(F("prevented text buffer overflow"));
		return;
	}
	length += strlcpy(&characters[length], str, len);
}