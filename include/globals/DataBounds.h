#ifndef DataBounds_h
#define DataBounds_h

#include <Arduino.h>
#include "globals/Typedefs.h"

// region Value range to bits
constexpr uint8_t VALUES_65536 = 16;
constexpr uint8_t VALUES_32768 = 15;
constexpr uint8_t VALUES_16384 = 14;
constexpr uint8_t VALUES_8192 = 13;
constexpr uint8_t VALUES_4096 = 12;
constexpr uint8_t VALUES_2048 = 11;
constexpr uint8_t VALUES_1024 = 10;
constexpr uint8_t VALUES_512 = 9;
constexpr uint8_t VALUES_256 = 8;
constexpr uint8_t VALUES_128 = 7;
constexpr uint8_t VALUES_64 = 6;
constexpr uint8_t VALUES_32 = 5;
constexpr uint8_t VALUES_16 = 4;
constexpr uint8_t VALUES_8 = 3;
constexpr uint8_t VALUES_4 = 2;
constexpr uint8_t VALUES_2 = 1;
// endregion

/// Static Game Engine Storage - calibration, UI, font data ========================================
/// (nothing written to a game save)
// Calibration data used for mapping points
constexpr uint16_t CALIB_MEMORY_START = 0;
constexpr uint16_t CALIB_TOTAL_STORAGE_SIZE = 29; // calibration uses first 29 values

// Color angles for color picker
constexpr uint16_t COLOR_MEMORY_START = CALIB_MEMORY_START + CALIB_TOTAL_STORAGE_SIZE;
constexpr uint16_t COLOR_TOTAL_STORAGE_SIZE = 144; // 12 x 12 bytes

// Font data is stored in eeprom
constexpr uint16_t FONT_MEMORY_START = COLOR_MEMORY_START + COLOR_TOTAL_STORAGE_SIZE;

/// Game Storage - objects, rooms, dialog, palettes (all written to a game save) ===================
const uint16_t startAddress = 4000; // TODO temporary to allow for font to expand

constexpr uint16_t objectCount = 84; // final number will be 950
constexpr uint16_t objectIdSize = VALUES_1024;
constexpr uint16_t objectStructSize = sizeof(Object);
constexpr uint16_t objectTotalStorageSize = objectStructSize * objectCount;
constexpr uint16_t objectMemoryStart = startAddress;

constexpr uint8_t roomCount = 36;
constexpr uint16_t roomIdSize = VALUES_64;
constexpr uint16_t objectsPerRoom = 130;
// (130*10) + 8 + 8 + 16 (objs as 10b per, palette, bgm, player as 16b)
constexpr uint16_t roomStructSize = 163 + 1 + 1 + 2;
constexpr uint16_t roomTotalStorageSize = roomStructSize * roomCount;
constexpr uint16_t roomMemoryStart = objectMemoryStart + objectTotalStorageSize;
const uint16_t roomPaletteAddr = 163;
const uint16_t roomMusicAddr = 163 + 1;
const uint16_t roomPlayerAddr = 163 + 1 + 1;

constexpr uint16_t dialogCount = 12; // final value will be 128
constexpr uint16_t dialogIdSize = VALUES_128; // 7b
constexpr uint16_t dialogMaxLength = 200;		 // arbitrary
constexpr uint16_t dialogStructSize = dialogMaxLength;
constexpr uint16_t dialogTotalStorageSize = dialogStructSize * dialogCount;
constexpr uint16_t dialogMemoryStart = roomMemoryStart + roomTotalStorageSize;

constexpr uint16_t paletteCount = 16;
constexpr uint16_t paletteIdSize = VALUES_16;
constexpr uint16_t paletteStructSize = 6; // 16b*3 colors = 48b/8 = 6
constexpr uint16_t paletteTotalStorageSize = paletteStructSize * paletteCount;
constexpr uint16_t paletteMemoryStart = dialogMemoryStart + dialogTotalStorageSize;

constexpr uint16_t titleAddress = paletteMemoryStart + paletteTotalStorageSize;
constexpr uint16_t titleMaxLength = 36;
constexpr uint16_t descriptionAddress = titleAddress + titleMaxLength;
constexpr uint16_t descriptionMaxLength = 36;
constexpr uint16_t directoryAddress = descriptionAddress + descriptionMaxLength;
constexpr uint16_t directoryMaxLength = 8; // DOS 13 byte max
constexpr uint16_t startingCoordsAddress = directoryAddress + directoryMaxLength;
constexpr uint16_t startingCoordsLength = 1 + 1 + 1; // x, y, room id
constexpr uint16_t startingDialogAddress = startingCoordsAddress + startingCoordsLength;
constexpr uint16_t startingDialogLength = dialogIdSize;

constexpr uint16_t endAddress = startingDialogAddress + startingDialogLength;

constexpr uint8_t musicCount = 36;

// not stored, all in ram at runtime and reset every game
constexpr uint8_t flagCount = 128;
constexpr uint8_t flagIdSize = (VALUES_128); // 7b

constexpr uint8_t _NO_FLAG = 0;
constexpr uint8_t _NO_GAMEOBJECT = 0;
constexpr uint8_t _NO_ROOM = 36;
constexpr uint8_t _NO_PALETTE = 0;
constexpr uint8_t _NO_DIALOG = 0;
constexpr uint8_t _NO_MUSIC = 0;
constexpr uint8_t _NO_GAME = 0;

// how much eeprom to load into ram
#define RAM_EEPROM_BYTE_COUNT (32000) // this is 256 kilobits


#endif
