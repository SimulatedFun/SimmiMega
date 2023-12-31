#ifndef Memory_h
#define Memory_h

#include <Arduino.h>
#include "globals/Typedefs.h"
#include "EEPROM_SPI_WE.h"
#include "globals/Pinout.h"
#include "Glyphs.h"
#include "GameObject.h"

#define EEPROM_BYTE_LEN 32000
#define EEPROM_SPI_FREQ 4000000

class ExtEeprom {
	private:
		SPIClass* _spi;
		EEPROM_SPI_WE* myEEP;
	public:
		explicit ExtEeprom(SPIClass* spi) : _spi(spi) {
			myEEP = new EEPROM_SPI_WE(spi, EEPROM_CS);
		}
		boolean initialize();
		void clear(uint32_t start = 0, uint32_t end = EEPROM_BYTE_LEN);
		void test();

		template <typename T>
		void write(T data, uint16_t address, uint16_t size);

		template <typename T>
		void read(T* data, uint16_t address, uint16_t size);
};

extern ExtEeprom* eeprom;
extern boolean debugEeprom;

#endif