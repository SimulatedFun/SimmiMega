#include "FileManager.h"

/// Count the number of folders with a "game.dat" file in it
uint8_t FileManager::countGameSavesTx() {
	File root = SD.open("/");
	if (!root) {
		Serial.println("Failed to open directory");
		return 0;
	}
	if (!root.isDirectory()) {
		Serial.println("Not a directory");
		return 0;
	}

	uint8_t count = 0;
	File file = root.openNextFile();
	while (file) {
		if (file.isDirectory()) {
			String filename = String(file.path()) + String("/game.dat");
			File gameData = SD.open(filename);
			if (gameData) {
				count++;
			}
		}

		file = root.openNextFile();
	}

	return count;
}

boolean FileManager::getSongInfoTx(uint8_t musicId, String* fileName) {
	Serial.printf("=========\ngetSongInfoTx %d\n", musicId);
	Folder gameFolder;
	GameSettings::getDirectory(&gameFolder);

	File musicFolder = SD.open("/" + String(gameFolder.text) + "/music", FILE_READ);
	if (!musicFolder) {
		Serial.printf("%s/music does not exist\n", gameFolder.text);
		musicFolder.close();
		return false;
	}
	if (!musicFolder.isDirectory()) {
		Serial.printf("%s/music not a directory\n", gameFolder.text);
		musicFolder.close();
		return false;
	}

	File file = musicFolder.openNextFile();
	while (file) {
		if (file.isDirectory()) {
			file.close();
			file = musicFolder.openNextFile();
			continue;
		}

		String localFileName = file.name();

		Serial.printf("looking for track %d from: %s\n", musicId, localFileName.c_str());

		char* dup = strdup(localFileName.c_str());
		char* trackNumber = strtok(dup, "_");

		Serial.printf("track number: %s\n", trackNumber);

		if (trackNumber == nullptr) {
			Serial.printf("failed to get track number for file %s\n", fileName->c_str());
			file.close();
			free(dup);
			file = musicFolder.openNextFile();
			continue;
		}
		// gets the 001 from '001_intro.wav'
		const uint8_t trackNumberInt = atoi(trackNumber);
		free(dup);

		// if it's the track we want, return the filename
		if (trackNumberInt == musicId) {
			Serial.printf("track number matched!\n");
			*fileName = file.name();
			file.close();
			musicFolder.close();
			return true;
		}
		file.close();
		file = musicFolder.openNextFile();
	}
	Serial.printf("track number %d not found...\n", musicId);
	musicFolder.close();
	return false;
}

boolean FileManager::saveRoomThumbnailData(uint8_t roomId) {
	// Generate the data to be inserted into the rmthumbs.dat file
	uint8_t data[130] = {0};
	Coordinates coords;
	GameObject obj(0);
	uint8_t iterator = 0;

	for (uint8_t y = 0; y < 10; y++) {
		for (uint8_t x = 0; x < 13; x++) {
			coords = Coordinates(x, y, roomId);
			RoomHelper::getGameObjectId(&obj.id, coords);
			obj.load();

			uint8_t depth = PALETTE_BACKGROUND;
			if (countSetBits(obj.data.frame1) >= 32) {
				if (obj.isHighlighted()) {
					depth = PALETTE_HIGHLIGHT;
				} else {
					depth = PALETTE_FOREGROUND;
				}
			}
			data[iterator++] = depth;
		}
	}

	microSd->begin();
	{
		Folder gameFolder;
		GameSettings::getDirectory(&gameFolder);

		File file = SD.open("/" + String(gameFolder.text) + "/rmthumbs.dat", FILE_WRITE);
		if (!file) {
			ERROR("unable to open room thumbnails data file");
			file.close();
			return false;
		}

		file.seek((uint64_t) roomId * roomStructSize);
		file.write(data, 130);
		file.close();
	}
	microSd->end();
	return true;
}

boolean FileManager::getRoomThumbnailDataTx(uint8_t roomId, uint8_t* data) {
	Folder gameFolder;
	GameSettings::getDirectory(&gameFolder);

	File file = SD.open("/" + String(gameFolder.text) + "/rmthumbs.dat", FILE_READ);
	if (!file) {
		ERROR("unable to open room thumbnails data file");
		file.close();
		return false;
	}

	file.seek(roomId * roomStructSize);
	file.read(data, roomStructSize);
	file.close();
	return true;
}

void FileManager::makeDirectoryIfNotExists(String path) {
	File file = SD.open(path, FILE_READ);
	if (!file) {
		file.close();
		SD.mkdir(path);
		return;
	}
}
