#ifndef Commands_h
#define Commands_h

#include <Arduino.h>
#include "spi/Touch.h"
#include "Glyphs.h"

class Commands {
	public:
		static void check();
		static void help();
};


#endif