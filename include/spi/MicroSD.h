#ifndef Storage_h
#define Storage_h

#include <Arduino.h>
#include <SPI.h>
#include "globals/Pinout.h"
#include "globals/SerialDebugging.h"
#include "GameData.h"

#include "FS.h"
#include "SD.h"

#define SPI_MICROSD_FREQUENCY 2500000
#define SD_RETRY_COUNT 10

using sd_err = uint8_t;
constexpr sd_err noSdError = 0;
constexpr sd_err errNoRoot = 1;
constexpr sd_err errFailMakeDir = 2;
constexpr sd_err errFailOpenDir = 3;
constexpr sd_err errFailOpenFile = 4;
constexpr sd_err errFailCloseFile = 5;
constexpr sd_err errFailRemove = 6;
constexpr sd_err errInvalidFilename = 7;
constexpr sd_err errSdNotInserted = 8;
constexpr sd_err errSdCannotBegin = 9;
constexpr sd_err errSdTransactionNotOpen = 10;
constexpr sd_err errFileInvalid = 11;
constexpr sd_err errFailReadFile = 12;
constexpr sd_err errFailNotExists = 13;

class MicroSD {
	private:
		SPIClass* _spi;
		boolean sdTransactionOpen = false;
	public:
		explicit MicroSD(SPIClass* spi) : _spi(spi) {}
		boolean isSDInserted(); // checks insertion pin for card physically present
		sd_err initialize(); // sets up spi, freq, etc. Call once.
		sd_err begin(); // opens sd transaction
		void end(); // closes sd transaction

		void test();

		// Directory related functions
		bool printAllInDirectory(File dir, uint8_t numTabs);
		bool setWorkingDirectoryToRoot();
		bool setWorkingDirectory(char* directoryName);

		// File related functions
		sd_err removeFile(char* filename);
		bool readLineFromFile(File file, uint16_t lineNumber, char* buffer, uint16_t bufferSize);
		bool fileExists(char* fileName);
		//bool closeSdFile(File* file); // just do file->close();
		void write64Bit(File* file, uint64_t number);

		boolean getSongInfo(uint8_t musicId, char* name);
};
extern MicroSD* microSd;

#endif