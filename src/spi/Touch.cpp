#include "spi/Touch.h"

void Touch::initialize() {
	pinMode(TOUCH_CS, OUTPUT);
	digitalWrite(TOUCH_CS, HIGH);
}

void Touch::getRawPressure(uint16_t* tz) {
	startRead();
	{
		// Z sample request
		*tz = 0xFFF;
		_spi->transfer(0xb0);               // Start new Z1 conversion
		*tz += _spi->transfer16(0xc0) >> 3;  // Read Z1 and start Z2 conversion
		*tz -= _spi->transfer16(0x00) >> 3;  // Read Z2
	}
	endRead();

	if (*tz == 4095) {
		*tz = 0;
	}
}

void Touch::getRawTouch(uint16_t* x, uint16_t* y) {
	uint16_t tmp;
	startRead();
	{
		// Start YP sample request for x position, read 4 times and keep last sample
		_spi->transfer(0b11010000); // Start new YP conversion
		_spi->transfer(0);		// Read first 8 bits
		_spi->transfer(0xd0); // Read last 8 bits and start new YP conversion
		_spi->transfer(0);		// Read first 8 bits
		_spi->transfer(0xd0); // Read last 8 bits and start new YP conversion
		_spi->transfer(0);		// Read first 8 bits
		_spi->transfer(0xd0); // Read last 8 bits and start new YP conversion

		tmp = _spi->transfer(0); // Read first 8 bits
		tmp = tmp << 5;
		tmp |= 0x1f & (_spi->transfer(0x90) >> 3); // Read last 8 bits and start new XP conversion

		*x = tmp;

		// Start XP sample request for y position, read 4 times and keep last sample
		_spi->transfer(0);		// Read first 8 bits
		_spi->transfer(0x90); // Read last 8 bits and start new XP conversion
		_spi->transfer(0);		// Read first 8 bits
		_spi->transfer(0x90); // Read last 8 bits and start new XP conversion
		_spi->transfer(0);		// Read first 8 bits
		_spi->transfer(0x90); // Read last 8 bits and start new XP conversion

		tmp = _spi->transfer(0); // Read first 8 bits
		tmp = tmp << 5;
		tmp |= 0x1f & (_spi->transfer(0) >> 3); // Read last 8 bits

		*y = tmp;
	}
	endRead();
}

void Touch::startRead() {
	_spi->beginTransaction(SPISettings(SPI_TOUCH_FREQUENCY, MSBFIRST, SPI_MODE0));
	digitalWrite(TOUCH_CS, LOW);
}

void Touch::endRead() {
	digitalWrite(TOUCH_CS, HIGH);
	_spi->endTransaction();
}