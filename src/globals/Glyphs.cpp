//#include "globals/Glyphs.h"
//
//namespace glyphs {
//	/// Gets a GlyphLookup object that tells you where in memory the glyph bitmap resides
//	/// Also retrieves relevant information such as how wide/high the glyph is, etc.
//	/// For glyph IDs, see asciiToGlyphLookupId(), but generally the structure is:
//	/// 0 - 61 are lower/upper case letters and digits
//	/// 62 - 82 are punctuation/ascii symbols
//	/// 82+ are icons used for UI (e.g. pencil icon, eraser, trash bin)
//	GlyphLookup getGlyphLookup(uint16_t glyphId) {
//		if (glyphId >= _RESERVED_SPECIAL_GLYPHS) {
//			Serial.printf("no glyph lookup for special glyphs");
//			return {};
//		}
//		/*
//		Order of font data goes:
//		- nintendo ascii bitmaps
//		- nintendo glyph lookups
//		- icon bitmaps
//		- icon glyph lookups
//		*/
//		GlyphLookup lookup;
//		uint32_t byteOffset = 0;
//
//		if (glyphId < NINTENDO_GLYPH_COUNT) { // letters and simple ascii characters
//			byteOffset = FONT_MEMORY_START + NINTENDO_BITMAP_BYTE_LEN;
//			//INFO("byteOffset: " << byteOffset);
//			ExtEeprom::read(&lookup, byteOffset + ((uint32_t) glyphId * GLYPH_STRUCT_SIZE), sizeof(GlyphLookup));
//			lookup.bitmapAddress += FONT_MEMORY_START;
//		} else if (glyphId < (NINTENDO_GLYPH_COUNT + ICONS_GLYPH_COUNT)) { // UI icons
//			byteOffset = FONT_MEMORY_START + NINTENDO_BITMAP_BYTE_LEN + NINTENDO_GLYPH_LOOKUP_BYTE_LEN + ICONS_BITMAP_BYTE_LEN;
//			//INFO("byteOffset: " << byteOffset);
//			ExtEeprom::read(&lookup, byteOffset + (((uint32_t) glyphId - ICON_GLYPH_0) * GLYPH_STRUCT_SIZE), sizeof(GlyphLookup));
//			//INFO("memory read bitmapAddress: " << lookup.bitmapAddress);
//			lookup.bitmapAddress += ICONS_MEMORY_START;
//			//INFO("memory read bitmapAddress plus offset: " << lookup.bitmapAddress);
//		}
//		//INFO("getGlyphLookup(glyphId: " << glyphId << ") -> addr: " << lookup.bitmapAddress << ", width: " << lookup.width << ", height: " << lookup.height << ", xAdvance: " << lookup.xAdvance);
//		return lookup;
//	}
//
//	/// Returns the exact pixel width this glyph would take up on screen at the
//	/// specified font size
//	uint8_t getGlyphPixelWidth(uint16_t glyphId, uint8_t fontSize = 1) {
//		if (glyphId >= _RESERVED_SPECIAL_GLYPHS) {
//			return 0;
//		}
//		const GlyphLookup lookup = getGlyphLookup(glyphId);
//		return lookup.xAdvance * fontSize;
//	}
//
//	/// Reads the glyph data from font memory, draws the glyph to the screen, wrapped in a transaction
//	/// and returns the pixel width (for cursor tracking)
//	/// \param x x coordinate of the top left corner of the glyph
//	/// \param y y coordinate of the top left corner of the glyph
//	/// \param glyphId id of the glyph to draw
//	/// \param color rgb565 formatted color
//	/// \param size size multiplier, 1 = normal, 2 = double, 3 = triple, etc
//	/// \return glyph width
//	uint8_t drawGlyphTx(uint16_t x, uint16_t y, uint16_t glyphId, uint16_t color, uint8_t size) {
//		//DEBUG("drawGlyphTx(" << x << ", " << y << ", " << glyphId << ", " << color << ", " << size << ")");
//		if (glyphId >= _RESERVED_SPECIAL_GLYPHS) {
//			ERROR("drawGlyphTx trying to draw a special glyph: " << glyphId);
//			return 0;
//		}
//		const GlyphLookup lookup = getGlyphLookup(glyphId);
//		uint32_t address = lookup.bitmapAddress;
//		uint8_t bits = 0;
//		uint8_t bit = 0;
//
//		for (uint8_t yy = 0; yy < lookup.height; yy++) {
//			for (uint8_t xx = 0; xx < lookup.width; xx++) {
//				const boolean a = (bit & 7) != 0;
//				bit++;
//				if (!a) { // determine if a new byte needs to be read
//					ExtEeprom::read(&bits, address, 1);
//					address++;
//				}
//				if ((bits & 128) != 0) {
//					const uint16_t xPos = x + (lookup.xOffset + xx) * size;
//					const uint16_t yPos = y + (lookup.yOffset + yy) * size;
//					Display::fillRectangleTx(xPos, yPos, size, size, color);
//				}
//				bits <<= 1;
//			}
//		}
//
//		return lookup.xAdvance * size; // returns the glyphWidth
//	}
//
////	/// Prints out a screen displaying all stored font data for debugging purposes
////	void debugFont() {
////		drawing_utils::fill(BLACK);
////		Display::startWrite();
////		{
////			uint8_t colWidth = 0;
////			uint8_t currWidth = 0;
////			uint16_t offset = 0;
////			uint16_t glyphId = 0;
////
////			for (uint8_t i = 0; i < 7; i++) {
////				for (uint8_t y = 0; y < 12; y++) {
////					if (glyphId >= NINTENDO_GLYPH_COUNT) {
////						continue;
////					}
////					currWidth = drawGlyphTx(5 + offset, 15 + y * 20, glyphId, WHITE, 1);
////					if (currWidth > colWidth) {
////						colWidth = currWidth;
////					}
////					glyphId++;
////				}
////				offset += colWidth + 6;
////				colWidth = 0;
////			}
////
////			glyphId = ICON_GLYPH_0;
////			for (uint8_t x = 0; x < 7; x++) {
////				for (uint8_t y = 0; y < 5; y++) {
////					if (glyphId >= (NINTENDO_GLYPH_COUNT + ICONS_GLYPH_COUNT)) {
////						continue;
////					}
////					currWidth = drawGlyphTx(5 + offset, 15 + y * 20, glyphId, WHITE, 1);
////					if (currWidth > colWidth) {
////						colWidth = currWidth;
////					}
////					glyphId++;
////				}
////				offset += colWidth + 6;
////				colWidth = 0;
////			}
////		}
////		Display::endWrite();
////
////		RAW("\n");
////		INFO(F("==== Font Storage ===="));
////		INFO(F("GlyphLookup struct size: ") << GLYPH_STRUCT_SIZE);
////		INFO(F("            --           "));
////		INFO(F("nintendo memory first address: ") << NINTENDO_MEMORY_START);
////		INFO(F("nintendo glyph count: ") << NINTENDO_GLYPH_COUNT);
////		INFO(F("nintendo bitmap byte length: ") << NINTENDO_BITMAP_BYTE_LEN);
////		INFO(F("nintendo lookup byte length: ") << NINTENDO_GLYPH_LOOKUP_BYTE_LEN);
////		INFO(F("nintendo total size: ") << NINTENDO_FONT_SIZE);
////		INFO(F("            --           "));
////		INFO(F("icon memory first address: ") << ICONS_MEMORY_START);
////		INFO(F("icon glyph count: ") << ICONS_GLYPH_COUNT);
////		INFO(F("icon bitmap byte length: ") << ICONS_BITMAP_BYTE_LEN);
////		INFO(F("icon lookup byte length: ") << ICONS_GLYPH_LOOKUP_BYTE_LEN);
////		INFO(F("icon total size: ") << ICONS_FONT_SIZE);
////		INFO(F("========================="));
////		RAW("\n");
////	}
//
//	/// Takes the font data arrays (output from binFont editor) and writes it to
//	/// external eeprom instead of storing it in RAM or in PROGMEM (flash space)
////	void storeFont() {
////		DEBUG("storeFont");
////
////		// Store lowercase and uppercase letter glyphs ("nintendo font")
////		// a-z are 0-25, A-Z are 26-31
////		// order: store bitmap data, then store the glyph lookup data
////		uint16_t address = 0;
////		DEBUG("storing nintendo bitmap");
////		for (uint32_t i = 0; i < NINTENDO_BITMAP_BYTE_LEN; i++) {
////			address = NINTENDO_MEMORY_START + i;
////			memory.write(nintendoBitmaps[i], address, 1);
////		}
////
////		DEBUG("storing nintendo glyphlookups");
////		for (uint32_t i = 0; i < NINTENDO_GLYPH_COUNT; i++) {
////			address = NINTENDO_MEMORY_START + NINTENDO_BITMAP_BYTE_LEN + (i * GLYPH_STRUCT_SIZE);
////			memory.write(nintendoGlyphs[i], address, sizeof(GlyphLookup));
////		}
////		DEBUG("stored nintendo font");
////
////		// order: store bitmap data, then store the glyph lookup data
////		DEBUG("storing icon bitmaps");
////		for (uint32_t i = 0; i < ICONS_BITMAP_BYTE_LEN; i++) {
////			address = ICONS_MEMORY_START + i;
////			memory.write(ui_iconsBitmaps[i], address, 1);
////		}
////		DEBUG("storing icon glyph lookups");
////		for (uint32_t i = 0; i < ICONS_GLYPH_COUNT; i++) {
////			address = ICONS_MEMORY_START + ICONS_BITMAP_BYTE_LEN + (i * GLYPH_STRUCT_SIZE);
////			memory.write(ui_iconsGlyphs[i], address, sizeof(GlyphLookup));
////		}
////		DEBUG("stored icons font");
////		DEBUG("done storeFont");
////	}
//} // namespace glyphs
