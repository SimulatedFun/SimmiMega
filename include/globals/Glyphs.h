///// The editor and engine use a font of letters and number for displaying information. There's also
///// the ui font which stores glyphs of the icons for rendering buttons. Both fonts are stored in
///// external EEPROM memory and accessed as needed.
///// The font is broken up into two parts:
///// - The raw bitmap data: an array of pixel data
///// - The individual glyph related information: width, height, bitmap offset of each glyph
//
///// To draw a glyph, we first find the GlyphLookup information. This will tell us a bitmapAddress,
///// which we can use to find the starting point of the bitmap information in the raw bitmap data.
///// We also know how many bytes to read based on the size of the glyph (width, height).
///// From there, we can draw the glyph.
//
//#ifndef glyphs_h
//#define glyphs_h
//
//#include <Arduino.h>
//#include "globals/ColorMacros.h"
//#include "globals/DataBounds.h"
//#include "globals/SerialDebugging.h"
//#include "spi/ExtEeprom.h"
//#include "spi/Display.h"
//
///// Font data stored PER GLYPH
//struct GlyphLookup {
//		uint16_t bitmapAddress = 0; ///< Actual memory address of bitmap for this glyph
//		uint8_t width = 0;			 ///< Bitmap dimensions in pixels
//		uint8_t height = 0;			 ///< Bitmap dimensions in pixels
//		uint8_t xAdvance = 0;		 ///< Distance to advance cursor (x axis)
//		int8_t xOffset = 0;			 ///< X dist from cursor pos to UL corner
//		int8_t yOffset = 0;			 ///< Y dist from cursor pos to UL corner
//
//		// Explicit constructor
//		GlyphLookup(uint16_t ba, uint8_t w, uint8_t h, uint8_t xa, int8_t xo, int8_t yo)
//			 : bitmapAddress(ba), width(w), height(h), xAdvance(xa), xOffset(xo), yOffset(yo) {}
//
//		// Default constructor
//		GlyphLookup() : bitmapAddress(0), width(0), height(0), xAdvance(0), xOffset(0), yOffset(0) {}
//};
//
//#include "fonts/SimmiNintendo.h"
//#include "fonts/SimmiUI.h"
//
//constexpr uint8_t GLYPH_STRUCT_SIZE = sizeof(GlyphLookup);
//
//// DS font
//constexpr uint16_t NINTENDO_MEMORY_START = (FONT_MEMORY_START);
//constexpr uint16_t NINTENDO_BITMAP_BYTE_LEN = (sizeof(nintendoBitmaps));
//constexpr uint16_t NINTENDO_GLYPH_LOOKUP_BYTE_LEN = (sizeof(nintendoGlyphs));
//constexpr uint16_t NINTENDO_GLYPH_COUNT = (sizeof(nintendoGlyphs) / GLYPH_STRUCT_SIZE);
//constexpr uint16_t NINTENDO_FONT_SIZE = (NINTENDO_BITMAP_BYTE_LEN + NINTENDO_GLYPH_LOOKUP_BYTE_LEN);
//
//// icons for UI, calibration, etc. a separate font will be for in-text symbols
//constexpr uint16_t ICONS_MEMORY_START = (FONT_MEMORY_START + NINTENDO_FONT_SIZE);
//constexpr uint16_t ICONS_BITMAP_BYTE_LEN = (sizeof(ui_iconsBitmaps));
//constexpr uint16_t ICONS_GLYPH_LOOKUP_BYTE_LEN = (sizeof(ui_iconsGlyphs));
//constexpr uint16_t ICONS_GLYPH_COUNT = (sizeof(ui_iconsGlyphs) / GLYPH_STRUCT_SIZE);
//constexpr uint16_t ICONS_FONT_SIZE = (ICONS_BITMAP_BYTE_LEN + ICONS_GLYPH_LOOKUP_BYTE_LEN);
//
//// FONT_MEMORY_END
//constexpr uint16_t FONT_MEMORY_END = (FONT_MEMORY_START + NINTENDO_FONT_SIZE + ICONS_FONT_SIZE);
//
//// region Glyph Addresses
//// system glyphs:
//constexpr uint8_t ICON_GLYPH_0 = (NINTENDO_GLYPH_COUNT);
//
//// clang-format off
//constexpr uint8_t QUESTION_OUTLINE    = (ICON_GLYPH_0 + 0);
//constexpr uint8_t QUESTION_WHITE      = (ICON_GLYPH_0 + 1);
//constexpr uint8_t QUESTION_SHADOW     = (ICON_GLYPH_0 + 2);
//constexpr uint8_t EXIT_OUTLINE        = (ICON_GLYPH_0 + 3);
//constexpr uint8_t EXIT_WHITE          = (ICON_GLYPH_0 + 4);
//constexpr uint8_t PENCIL_OUTLINE      = (ICON_GLYPH_0 + 5);
//constexpr uint8_t PENCIL_WHITE        = (ICON_GLYPH_0 + 6);
//constexpr uint8_t PENCIL_SHADOW       = (ICON_GLYPH_0 + 7);
//constexpr uint8_t ERASER_OUTLINE      = (ICON_GLYPH_0 + 8);
//constexpr uint8_t ERASER_WHITE        = (ICON_GLYPH_0 + 9);
//constexpr uint8_t ERASER_SHADOW       = (ICON_GLYPH_0 + 10);
//constexpr uint8_t COPY_OUTLINE        = (ICON_GLYPH_0 + 11);
//constexpr uint8_t COPY_WHITE          = (ICON_GLYPH_0 + 12);
//constexpr uint8_t COPY_SHADOW         = (ICON_GLYPH_0 + 13);
//constexpr uint8_t PASTE_OUTLINE       = (ICON_GLYPH_0 + 14);
//constexpr uint8_t PASTE_WHITE         = (ICON_GLYPH_0 + 15);
//constexpr uint8_t PASTE_SHADOW        = (ICON_GLYPH_0 + 16);
//constexpr uint8_t TRASH_OUTLINE       = (ICON_GLYPH_0 + 17);
//constexpr uint8_t TRASH_WHITE         = (ICON_GLYPH_0 + 18);
//constexpr uint8_t TRASH_SHADOW        = (ICON_GLYPH_0 + 19);
//constexpr uint8_t CALIBRATION_MARKER  = (ICON_GLYPH_0 + 20);
//constexpr uint8_t LEFT_ARROW_OUTLINE  = (ICON_GLYPH_0 + 21);
//constexpr uint8_t LEFT_ARROW_WHITE    = (ICON_GLYPH_0 + 22);
//constexpr uint8_t RIGHT_ARROW_OUTLINE = (ICON_GLYPH_0 + 23);
//constexpr uint8_t RIGHT_ARROW_WHITE   = (ICON_GLYPH_0 + 24);
//constexpr uint8_t KB_BACKSPACE_ARROW  = (ICON_GLYPH_0 + 25);
//constexpr uint8_t KB_ENTER_ARROW      = (ICON_GLYPH_0 + 26);
//constexpr uint8_t KB_HEART_OUTLINE    = (ICON_GLYPH_0 + 27);
//constexpr uint8_t KB_HEART_WHITE      = (ICON_GLYPH_0 + 28);
//constexpr uint8_t KB_SMILEY_OUTLINE   = (ICON_GLYPH_0 + 29);
//constexpr uint8_t KB_SMILEY_WHITE     = (ICON_GLYPH_0 + 30);
//constexpr uint8_t KB_PALETTE_OUTLINE  = (ICON_GLYPH_0 + 31);
//constexpr uint8_t KB_PALETTE_LEFT     = (ICON_GLYPH_0 + 32);
//constexpr uint8_t KB_PALETTE_TOP      = (ICON_GLYPH_0 + 33);
//constexpr uint8_t KB_PALETTE_RIGHT    = (ICON_GLYPH_0 + 34);
//constexpr uint8_t INVERT_OUTLINE      = (ICON_GLYPH_0 + 35);
//constexpr uint8_t INVERT_WHITE        = (ICON_GLYPH_0 + 36);
//constexpr uint8_t LIGHTBULB_OUTLINE   = (ICON_GLYPH_0 + 37);
//constexpr uint8_t LIGHTBULB_WHITE     = (ICON_GLYPH_0 + 38);
//constexpr uint8_t LIGHTBULB_SHADOW    = (ICON_GLYPH_0 + 39);
//constexpr uint8_t DOOR_ICON           = (ICON_GLYPH_0 + 40);
//constexpr uint8_t FLAG_ICON           = (ICON_GLYPH_0 + 41);
//constexpr uint8_t PLAY_OUTLINE        = (ICON_GLYPH_0 + 42);
//constexpr uint8_t PLAY_GREEN          = (ICON_GLYPH_0 + 43);
//constexpr uint8_t CHECKMARK_WHITE     = (ICON_GLYPH_0 + 44);
//constexpr uint8_t CHECKMARK_OUTLINE   = (ICON_GLYPH_0 + 45);
//
//constexpr uint8_t GLYPH_END_OF_LINE = 255 - 1;
//constexpr uint8_t GLYPH_NEW_LINE = 255 - 2;
//constexpr uint8_t GLYPH_COLOR_15 = 255 - 3;
//constexpr uint8_t GLYPH_COLOR_14 = 255 - 4;
//constexpr uint8_t GLYPH_COLOR_13 = 255 - 5;
//constexpr uint8_t GLYPH_COLOR_12 = 255 - 6;
//constexpr uint8_t GLYPH_COLOR_11 = 255 - 7;
//constexpr uint8_t GLYPH_COLOR_10 = 255 - 8;
//constexpr uint8_t GLYPH_COLOR_9 = 255 - 9;
//constexpr uint8_t GLYPH_COLOR_8 = 255 - 10;
//constexpr uint8_t GLYPH_COLOR_7 = 255 - 11;
//constexpr uint8_t GLYPH_COLOR_6 = 255 - 12;
//constexpr uint8_t GLYPH_COLOR_5 = 255 - 13;
//constexpr uint8_t GLYPH_COLOR_4 = 255 - 14;
//constexpr uint8_t GLYPH_COLOR_3 = 255 - 15;
//constexpr uint8_t GLYPH_COLOR_2 = 255 - 16;
//constexpr uint8_t GLYPH_COLOR_1 = 255 - 17;
//constexpr uint8_t _RESERVED_SPECIAL_GLYPHS = 255 - 18;
//
//// clang-format on
//// endregion
//
//namespace glyphs {
//	GlyphLookup getGlyphLookup(uint16_t glyphId);
//	uint8_t getGlyphPixelWidth(uint16_t glyphId, uint8_t fontSize);
//	uint8_t drawGlyphTx(uint16_t x, uint16_t y, uint16_t glyphId, uint16_t color, uint8_t size);
//	void debugFont();
//	void storeFont();
//} // namespace glyphs
//
//#endif // glyphs_h