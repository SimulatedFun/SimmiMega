#ifndef Memory_h
#define Memory_h

#include "EEPROM_SPI_WE.h"
#include "globals/Pinout.h"

class ExtEeprom {
	public:
		static void initialize();
		static void test();

		template <typename T>
		static void write(T data, uint16_t address, uint16_t size);

		template <typename T>
		static void read(T* data, uint16_t address, uint16_t size);
};

extern boolean debugEeprom;

#endif