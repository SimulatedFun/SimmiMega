#ifndef Memory_h
#define Memory_h

#include "EEPROM_SPI_WE.h"
#include "globals/pinout.h"

class ExtEeprom {
public:
	static void initialize();
	static void test();
};

#endif