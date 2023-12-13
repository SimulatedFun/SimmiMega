#ifndef Touch_h
#define Touch_h

#include <Arduino.h>
#include <SPI.h>
#include "globals/Pinout.h"

#define SPI_TOUCH_FREQUENCY 2500000

class Touch {
	private:
		SPIClass* _spi;
		void startRead();
		void endRead();
	public:
		explicit Touch(SPIClass* spi) : _spi(spi) {}
		void initialize();
		void getRawTouch(uint16_t* x, uint16_t* y);
		void getRawPressure(uint16_t* tz);
};

#endif