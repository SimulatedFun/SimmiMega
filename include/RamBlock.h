#ifndef RamBlock_h
#define RamBlock_h

// This will represent a static block of SRAM memory meant to mimic the structure of the EEPROM so
// we can call/index it with the same functions. When the device powers on, I just want to copy all
// data in eeprom to this block of memory in ram. That way, in play mode, it's very quick to
// retrieve data.

#include <Arduino.h>
#include "GameObject.h"
#include "Palette.h"

#define RAM_BYTE_LEN 32000

class RamBlock {
	public:
		void initialize();

		template <typename T>
		void write(T data, uint16_t address, uint16_t size);

		template <typename T>
		void read(T* data, uint16_t address, uint16_t size);
};

extern RamBlock* ram;
extern boolean debugRam;

#endif