#ifndef Storage_h
#define Storage_h

#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "globals/Pinout.h"

class MicroSD {
	public:
		static void initialize();
		static void test();
};

#endif