#ifndef Storage_h
#define Storage_h

#include <Arduino.h>
#include <SPI.h>
#include "globals/Pinout.h"

#include "FS.h"
#include "SD.h"

#define SPI_MICROSD_FREQUENCY 2500000

class MicroSD {
	private:
		SPIClass* _spi;
	public:
		explicit MicroSD(SPIClass* spi) : _spi(spi) {}
		void initialize();
		void test();
};

#endif