#ifndef Audio_h
#define Audio_h

#include <Arduino.h>
#include <SPI.h>
class Audio {
	public:
		Audio() = default;
		static void initialize();
		static void buffer();
		static void stop();
		static void playFile(char* filename);
};
#endif