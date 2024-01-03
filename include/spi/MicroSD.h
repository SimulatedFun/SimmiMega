#ifndef Storage_h
#define Storage_h

#include <Arduino.h>
#include <SPI.h>
#include <cstdint>
#include "globals/Pinout.h"
#include "globals/DataBounds.h"
#include "globals/Typedefs.h"

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
	public:
		boolean sdTransactionOpen = false;
		explicit MicroSD(SPIClass* spi) : _spi(spi) {}
		boolean isSDInserted(); // checks insertion pin for card physically present
		sd_err initialize(); // sets up spi, freq, etc. Call once.
		sd_err begin(); // opens sd transaction
		void end(); // closes sd transaction

		// Fundamental operations
		void createDirTx(str path);
		void readFile(str path);
		void listDirTx(str dirname, uint8_t levels = 10);
		void appendFileTx(const String& path, const String& message);
};
extern MicroSD* microSd;

#endif