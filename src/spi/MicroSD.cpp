#include "spi/MicroSD.h"

// todo reimplement working directory/root so I can port sd code from another project

void MicroSD::listDirTx(const String& dirname, uint8_t levels) {
	Serial.printf("Listing directory: %s\n", dirname.c_str());

	File root = SD.open(dirname);
	if (!root) {
		Serial.println("Failed to open directory");
		return;
	}
	if (!root.isDirectory()) {
		Serial.println("Not a directory");
		return;
	}

	File file = root.openNextFile();
	while (file) {
		if (file.isDirectory()) {
			Serial.print("  DIR : ");
			Serial.println(file.name());
			if (levels != 0) {
				listDirTx(file.path(), levels - 1);
			}
		} else {
			Serial.print("  FILE: ");
			Serial.print(file.name());
			Serial.print("  SIZE: ");
			Serial.println(file.size());
		}
		file = root.openNextFile();
	}
}

void MicroSD::createDirTx(const String& path) {
	Serial.printf("Creating Dir: %s\n", path.c_str());
	if (SD.mkdir("/" + path)) {
		Serial.println("Dir created");
	} else {
		Serial.println("mkdir failed");
	}
}

void MicroSD::removeDirTx(const String& path) {
	Serial.printf("Removing Dir: %s\n", path.c_str());
	if (SD.rmdir(path)) {
		Serial.println("Dir removed");
	} else {
		Serial.println("rmdir failed");
	}
}

void MicroSD::readFileTx(const String& path) {
	Serial.printf("Reading file: %s\n", path.c_str());

	File file = SD.open(path);
	if (!file) {
		Serial.println("Failed to open file for reading");
		return;
	}

	while (file.available() != 0) {
		Serial.write(file.read());
	}
	file.close();
}

void MicroSD::appendFileTx(const String& path, const String& message) {
	Serial.printf("Appending to file: %s : %s\n", path.c_str(), message.c_str());

	File file = SD.open(path, FILE_APPEND);
	if (!file) {
		Serial.println("Failed to open file for appending");
		file.close();
		return;
	}

	if (file.print(message) != 0) {
		Serial.println("Message appended");
	} else {
		Serial.println("Append failed");
	}
	file.close();
}

void MicroSD::renameFileTx(const String& path1, const String& path2) {
	Serial.printf("Renaming file %s to %s\n", path1.c_str(), path2.c_str());
	if (SD.rename(path1, path2)) {
		Serial.println("File renamed");
	} else {
		Serial.println("Rename failed");
	}
}

void MicroSD::deleteFileTx(const String& path) {
	Serial.printf("Deleting file: %s\n", path.c_str());
	if (SD.remove(path)) {
		Serial.println("File deleted");
	} else {
		Serial.println("Delete failed");
	}
}

boolean MicroSD::isSDInserted() {
	return true; // todo implement checking pin
}

sd_err MicroSD::initialize() {
	// Pull high to disable SD card
	pinMode(SD_CS, OUTPUT);
	digitalWrite(SD_CS, HIGH);

	// attempt an SD transaction open/close
    sd_err err = noSdError;
    //	err = begin();
    //	end();

	return err;
}

sd_err MicroSD::begin() {
	// check if card is physically inserted
	if (!isSDInserted()) {
		return errSdNotInserted;
	}

	// check if transaction is already open
	if (sdTransactionOpen) {
		end();
	}

	// attempt an SD transaction open/close
	for (uint8_t i = 0; i <= SD_RETRY_COUNT; i++) {
		const boolean sdStarted = SD.begin(SD_CS, *_spi, SPI_MICROSD_FREQUENCY);
		if (sdStarted) {
			sdTransactionOpen = true;
			return noSdError;
		}
		Serial.printf("SD init failed. Trying %d more times.", SD_RETRY_COUNT - i);
		delay(750);
	}

	end();
	return errSdCannotBegin;
}

void MicroSD::end() {
	SD.end();
	sdTransactionOpen = false;
}
