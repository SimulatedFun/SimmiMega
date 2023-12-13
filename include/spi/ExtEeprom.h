#ifndef Memory_h
#define Memory_h

#include <Arduino.h>
#include "EEPROM_SPI_WE.h"
#include "globals/Pinout.h"

#define EEPROM_LAST_ADDRESS (32000-1)

class ExtEeprom {
	private:
		SPIClass* _spi;
		EEPROM_SPI_WE* myEEP;
	public:
		explicit ExtEeprom(SPIClass* spi) : _spi(spi) {
			myEEP = new EEPROM_SPI_WE(spi, EEPROM_CS);
		}
		void initialize();
		void test();

		template <typename T>
		void write(T data, uint16_t address, uint16_t size);

		template <typename T>
		void read(T* data, uint16_t address, uint16_t size);
};

extern ExtEeprom* eeprom;
extern boolean debugEeprom;

#endif