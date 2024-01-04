#include "RoomHelper.h"
void RoomHelper::getGameObjectId(uint16_t* gameObjectId, Coordinates coord, boolean fromRam) {
	const uint16_t address = roomMemoryStart + (coord.roomId * roomStructSize);
	int index = (coord.y * 13 + coord.x) * 10;

	int num_bits = 10;
	unsigned int mask = (1 << num_bits) - 1;
	int byte_index = index / 8;
	int bit_offset = index % 8;

	unsigned char byte1 = 0;
    if (fromRam) {
        ram->read(&byte1, address + byte_index, 1);
    } else {
        eeprom->read(&byte1, address + byte_index, 1);
    }
	unsigned char byte2 = 0;
    if (fromRam) {
        ram->read(&byte2, address + byte_index + 1, 1);
    } else {
        eeprom->read(&byte2, address + byte_index + 1, 1);
    }

	unsigned int low_byte = byte1 >> bit_offset;
	unsigned int high_byte = byte2 << (8 - bit_offset);

	*gameObjectId = (high_byte | low_byte) & mask;
}
void RoomHelper::setGameObjectId(uint16_t gameObjectId, Coordinates coord) {
	const uint16_t address = roomMemoryStart + (coord.roomId * roomStructSize);
	int index = (coord.y * 13 + coord.x) * 10;

	int num_bits = 10;
	unsigned int mask = (1 << num_bits) - 1;
	unsigned int value = gameObjectId & mask; // Ensure value is within bit limit

	int byte_index = index / 8;
	int bit_offset = index % 8;

	unsigned char byte1 = 0;
	eeprom->read(&byte1, address + byte_index, 1);
	unsigned char byte2 = 0;
	eeprom->read(&byte2, address + byte_index + 1, 1);

	byte1 &= ~(mask << bit_offset); // Clear the bits at the insertion point
	byte1 |= (value << bit_offset); // Set the bits with the new value

	byte2 &= ~(mask >> (8 - bit_offset)); // Clear the bits that will be in byte2
	byte2 |= (value >> (8 - bit_offset)); // Set the bits in byte2

	eeprom->write(byte1, address + byte_index, 1);
	eeprom->write(byte2, address + byte_index + 1, 1);

	ram->write(byte1, address + byte_index, 1);
	ram->write(byte2, address + byte_index + 1, 1);
}

void RoomHelper::getPaletteId(uint8_t* paletteId, uint8_t roomId, boolean fromRam) {
	const uint16_t address = roomMemoryStart + (roomId * roomStructSize) + roomPaletteAddr;
    if (fromRam) {
        ram->read(paletteId, address, 1);
    } else {
        eeprom->read(paletteId, address, 1);
    }
	if (*paletteId >= paletteCount) {
		Serial.printf("palette id OOB: %d\n", *paletteId);
		*paletteId = _NO_PALETTE;
	}
}
void RoomHelper::setPaletteId(uint8_t paletteId, uint8_t roomId) {
	if (paletteId >= paletteCount) {
		Serial.printf("palette id OOB: %d\n", paletteId);
		paletteId = _NO_PALETTE;
	}

	const uint16_t address = roomMemoryStart + (roomId * roomStructSize) + roomPaletteAddr;
	eeprom->write(paletteId, address, 1);
	ram->write(paletteId, address, 1);
}

void RoomHelper::getPlayerGameObjectId(uint16_t* playerId, uint8_t roomId, boolean fromRam) {
	const uint16_t address = roomMemoryStart + (roomId * roomStructSize) + roomPlayerAddr;
    if (fromRam) {
        ram->read(playerId, address, 2);
    } else {
        eeprom->read(playerId, address, 2);
    }
	if (*playerId >= objectCount) {
		Serial.printf("player gameobj id OOB: %d for roomId %d\n", *playerId, roomId);
		*playerId = _NO_GAMEOBJECT;
	}
}
void RoomHelper::setPlayerGameObjectId(uint16_t playerId, uint8_t roomId) {
	if (playerId >= objectCount) {
		Serial.printf("player gameobj id OOB: %d for roomId %d\n", playerId, roomId);
		playerId = _NO_GAMEOBJECT;
	}
	const uint16_t address = roomMemoryStart + (roomId * roomStructSize) + roomPlayerAddr;
	eeprom->write(playerId, address, 2);
	ram->write(playerId, address, 2);
}

void RoomHelper::getMusicId(uint8_t* musicId, uint8_t roomId, boolean fromRam) {
	const uint16_t address = roomMemoryStart + (roomId * roomStructSize) + roomMusicAddr;
    if (fromRam) {
        ram->read(musicId, address, 1);
    } else {
	    eeprom->read(musicId, address, 1);
    }
	if (*musicId >= musicCount) {
		Serial.printf("bgm track OOB: %d\n", *musicId);
		*musicId = _NO_MUSIC;
	}
}
void RoomHelper::setMusicId(uint8_t musicId, uint8_t roomId) {
	if (musicId >= musicCount) {
		Serial.printf("bgm track OOB: %d\n", musicId);
		musicId = _NO_MUSIC;
	}
	if (roomId >= roomCount) {
		Serial.printf("bgm track OOB: %d\n", musicId);
		return;
	}
	const uint16_t address = roomMemoryStart + (roomId * roomStructSize) + roomMusicAddr;
	eeprom->write(musicId, address, 1);
	ram->write(musicId, address, 1);
}
/// Prints out all objects nicely to console in a room
/// \param rmId
void RoomHelper::roomToString(const uint8_t rmId) {
#ifdef ENABLE_TO_STRING
#ifdef ENABLE_SERIAL
	Serial << "+----------------------------------------------------------+" << endl;
	Serial << "Room ID = " << rmId << endl;
	uint16_t gid = 0;
	Coordinates coordinates;
	coordinates.roomId = rmId;
	for (uint8_t y = 0; y < 10; y++) {
		for (uint8_t x = 0; x < 13; x++) {
			coordinates.x = x;
			coordinates.y = y;
			getGameObjectId(&gid, coordinates);
			if (gid <= 9) {
				Serial << gid << "     ";
				continue;
			}
			if (gid <= 99) {
				Serial << gid << "    ";
				continue;
			}
			if (gid <= 999) {
				Serial << gid << "   ";
				continue;
			}
			if (gid <= 9999) {
				Serial << gid << "  ";
				continue;
			}
			Serial << gid << " "; // up to 65535
		}
		Serial << endl;
	}
	Serial << "+----------------------------------------------------------+" << endl << endl;
#endif
#endif
}
