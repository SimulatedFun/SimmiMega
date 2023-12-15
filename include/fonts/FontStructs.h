#ifndef FontStructs_h
#define FontStructs_h

#include <Arduino.h>

/// Glyph lookup
typedef struct {
		uint16_t bitmapAddress; ///< Pointer into GFXfont->bitmap
		uint8_t width;         ///< Bitmap dimensions in pixels
		uint8_t height;        ///< Bitmap dimensions in pixels
		uint8_t xAdvance;      ///< Distance to advance cursor (x axis)
		int8_t xOffset;        ///< X dist from cursor pos to UL corner
		int8_t yOffset;        ///< Y dist from cursor pos to UL corner
} GlyphLookup;

/// Font metadata with pointer to bitmaps and glyph lookups
typedef struct {
		uint8_t *bitmap;  ///< Glyph bitmaps, concatenated
		GlyphLookup* glyph;  ///< Glyph array
		uint16_t first;   ///< ASCII extents (first char)
		uint16_t last;    ///< ASCII extents (last char)
		uint8_t yAdvance; ///< Newline distance (y axis)
} GFXfont;

extern GFXfont* currentFont;

#endif