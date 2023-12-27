#ifndef FileManager_h
#define FileManager_h

#include <Arduino.h>
#include "spi/MicroSD.h"
#include "globals/Typedefs.h"
#include "GameSettings.h"
#include "RoomHelper.h"

namespace FileManager {
	// Gets the filename corresponding to the music ID
	boolean getSongInfoTx(uint8_t musicId, String* fileName);

	// Given a room ID, returns the thumbnail data for drawing in the room selector
	boolean getRoomThumbnailDataTx(uint8_t roomId, uint8_t* data);
	boolean saveRoomThumbnailData(uint8_t roomId);

	uint8_t countGameSavesTx();

	void makeDirectoryIfNotExists(String path);

} // namespace FileManager

#endif