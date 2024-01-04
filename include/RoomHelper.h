#ifndef Room_h
#define Room_h

#include <Arduino.h>
#include "Coordinates.h"
#include "spi/ExtEeprom.h"
#include "RamBlock.h"
#include "globals/DataBounds.h"

class RoomHelper {
	public:
		static void roomToString(uint8_t roomId);
        static void getGameObjectId(uint16_t* gameObjectId, Coordinates coordinates, boolean fromRam = false);
        static void setGameObjectId(uint16_t gameObjectId, Coordinates coordinates);
        static void getPaletteId(uint8_t* paletteId, uint8_t roomId, boolean fromRam = false);
        static void setPaletteId(uint8_t paletteId, uint8_t roomId);
        static void getPlayerGameObjectId(uint16_t* playerId, uint8_t roomId, boolean fromRam = false);
        static void setPlayerGameObjectId(uint16_t playerId, uint8_t roomId);
        static void getMusicId(uint8_t* musicId, uint8_t roomId, boolean fromRam = false);
        static void setMusicId(uint8_t musicId, uint8_t roomId);
};

#endif
