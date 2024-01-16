#ifndef Memory_h
#define Memory_h

#include <Arduino.h>
#include "globals/Typedefs.h"
#include "EEPROM_SPI_WE.h" // 3rd party library
#include "globals/Pinout.h"
#include "Glyphs.h"
#include "GameObject.h"

#define EEPROM_BYTE_LEN 32000 // size of the SPI chip (25LC256 = 256000 bits = 32000 bytes)
#define EEPROM_SPI_FREQ 4000000 // 40MHz is what the chip is rated for in the data sheet

class ExtEeprom {
	private:
		SPIClass* _spi;		 // shared spi bus
		EEPROM_SPI_WE* myEEP; // external eeprom library object pointer
	public:
		explicit ExtEeprom(SPIClass* spi);

		/// Initializes the pins and sets all the appropriate eeprom settings
		/// \return returns true if the eeprom initializes without issue
		boolean initialize();

		/// Completely erases all data on the entire eeprom chip
		/// NOTE: this is used in the "new project" area
		/// \param start start address in eeprom to erase (defaults to 0)
		/// \param end end address in eeprom to erase (defaults to last byte)
		void clear(uint32_t start = 0, uint32_t end = EEPROM_BYTE_LEN);

		/// Example function that shows how to use the various functions from the library
		/// TODO: Delete and replace with a test suite
		void exampleCode();

		/// Takes some data, forcefully reinterpret casts it to a byte array, and writes it to eeprom.
		/// \tparam T any type of data: uint8_t, uint16_t, data array, string, etc.
		/// \param data data to be stored
		/// \param address address in eeprom to store the data
		/// \param size num of bytes to write
		template<typename T>
		void write(T data, uint16_t address, uint16_t size);

		/// Forcefully takes data from eeprom and puts it into the data pointer.
		/// \tparam T any type of data: uint8_t, uint16_t, data array, string, etc.
		/// \param data data to be read
		/// \param address address in eeprom to read the data from
		/// \param size num of bytes to read
		template<typename T>
		void read(T* data, uint16_t address, uint16_t size);
};

extern ExtEeprom* eeprom;
extern boolean debugEeprom; // set to true to show read/write messages with address and byte count

#endif