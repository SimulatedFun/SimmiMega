#ifndef DataBounds_h
#define DataBounds_h

#include <Arduino.h>
#include "globals/Typedefs.h"

// region Value range to bits
constexpr u8 VALUES_65536 = 16;
constexpr u8 VALUES_32768 = 15;
constexpr u8 VALUES_16384 = 14;
constexpr u8 VALUES_8192 = 13;
constexpr u8 VALUES_4096 = 12;
constexpr u8 VALUES_2048 = 11;
constexpr u8 VALUES_1024 = 10;
constexpr u8 VALUES_512 = 9;
constexpr u8 VALUES_256 = 8;
constexpr u8 VALUES_128 = 7;
constexpr u8 VALUES_64 = 6;
constexpr u8 VALUES_32 = 5;
constexpr u8 VALUES_16 = 4;
constexpr u8 VALUES_8 = 3;
constexpr u8 VALUES_4 = 2;
constexpr u8 VALUES_2 = 1;
// endregion

/// Static Game Engine Storage - calibration, UI, font data ========================================
/// (nothing written to a game save)
// Calibration data used for mapping points
constexpr u16 CALIB_MEMORY_START = 0;
constexpr u16 CALIB_TOTAL_STORAGE_SIZE = 29; // calibration uses first 29 values

/// Game Storage - objects, rooms, dialog, palettes (all written to a game save) ===================
constexpr u16 startAddress = CALIB_MEMORY_START + CALIB_TOTAL_STORAGE_SIZE;

constexpr u16 objectCount = 84; // final number will be 950
constexpr u16 objectIdSize = VALUES_1024;
constexpr u16 objectStructSize = sizeof(Object);
constexpr u16 objectTotalStorageSize = objectStructSize * objectCount;
constexpr u16 objectMemoryStart = startAddress;

constexpr u8 roomCount = 36;
constexpr u16 roomIdSize = VALUES_64;
constexpr u16 objectsPerRoom = 130;
// (130*10) + 8 + 8 + 16 (objs as 10b per, palette, bgm, player as 16b)
constexpr u16 roomStructSize = 163 + 1 + 1 + 2;
constexpr u16 roomTotalStorageSize = roomStructSize * roomCount;
constexpr u16 roomMemoryStart = objectMemoryStart + objectTotalStorageSize;
constexpr u16 roomPaletteAddr = 163;
constexpr u16 roomMusicAddr = 163 + 1;
constexpr u16 roomPlayerAddr = 163 + 1 + 1;

constexpr u16 dialogCount = 12; // final value will be 128
constexpr u16 dialogIdSize = VALUES_128; // 7b
constexpr u16 dialogMaxLength = 200;		 // arbitrary
constexpr u16 dialogStructSize = dialogMaxLength;
constexpr u16 dialogTotalStorageSize = dialogStructSize * dialogCount;
constexpr u16 dialogMemoryStart = roomMemoryStart + roomTotalStorageSize;

constexpr u16 paletteCount = 32; // including zero
constexpr u16 paletteIdSize = VALUES_32;
constexpr u16 paletteStructSize = 6; // 16b*3 colors = 48b/8 = 6
constexpr u16 paletteTotalStorageSize = paletteStructSize * paletteCount;
constexpr u16 paletteMemoryStart = dialogMemoryStart + dialogTotalStorageSize;

constexpr u16 titleAddress = paletteMemoryStart + paletteTotalStorageSize;
constexpr u16 titleMaxLength = 36;
constexpr u16 descriptionAddress = titleAddress + titleMaxLength;
constexpr u16 descriptionMaxLength = 36;
constexpr u16 directoryAddress = descriptionAddress + descriptionMaxLength;
constexpr u16 directoryMaxLength = 8; // DOS 13 byte max
constexpr u16 startingCoordsAddress = directoryAddress + directoryMaxLength;
constexpr u16 startingCoordsLength = 1 + 1 + 1; // x, y, room id
constexpr u16 startingDialogAddress = startingCoordsAddress + startingCoordsLength;
constexpr u16 startingDialogLength = dialogIdSize;

constexpr u16 endAddress = startingDialogAddress + startingDialogLength;

constexpr u8 musicCount = 36;

// not stored, all in ram at runtime and reset every game
constexpr u8 flagCount = 128;
constexpr u8 flagIdSize = (VALUES_128); // 7b

constexpr u8 _NO_FLAG = 0;
constexpr u8 _NO_GAMEOBJECT = 0;
constexpr u8 _NO_ROOM = 36;
constexpr u8 _NO_PALETTE = 0;
constexpr u8 _NO_DIALOG = 0;
constexpr u8 _NO_MUSIC = 0;
constexpr u8 _NO_GAME = 0;

// how much eeprom to load into ram
constexpr u32 RAM_EEPROM_BYTE_COUNT = 32000; // this is 256 kilobits


#endif
