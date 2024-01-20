#ifndef Audio_h
#define Audio_h

#include <Arduino.h>
#include <SPI.h>
#include "spi/MicroSD.h"
#include "Audio.h"
#include "globals/SerialDebugging.h"

// The max98357a acts as a 3-watt amplifier

class Sound {
	public:
		Sound() = default;
		static void initialize();
		static void buffer();
		static void stop();
		static void playFile(const String& filename);
};

#endif