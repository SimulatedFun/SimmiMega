#include "spi/Display.h"

void Display::startWrite() {
	_spi->beginTransaction(SPISettings(SPI_TFT_FREQUENCY, MSBFIRST, SPI_MODE0));
	digitalWrite(TFT_CS, LOW);
}

void Display::endWrite() {
	digitalWrite(TFT_CS, HIGH);
	_spi->endTransaction();
}

void Display::writeCommand(uint8_t cmd) {
	digitalWrite(TFT_DC, LOW);
	_spi->write(cmd);
	digitalWrite(TFT_DC, HIGH);
}

void Display::initialize() {
	pinMode(TFT_DC, OUTPUT);
	digitalWrite(TFT_DC, HIGH);

	// cs needs pull-up resistor to program/boot properly
	pinMode(TFT_CS, OUTPUT);
	digitalWrite(TFT_CS, HIGH);

	pinMode(TFT_RST, OUTPUT);
	digitalWrite(TFT_RST, HIGH);

	sendCommand(ILI9341_SWRESET);
	delay(150);

	// Processes all needed commands for boot-up from the tftCommands array
	uint16_t addressOffset = 0;
	uint8_t commands[15];
	while (addressOffset < bootCommandLength) {
		const uint8_t cmd = pgm_read_byte_near(tftBootCommands + addressOffset);
		addressOffset++;
		const uint8_t numArgs = pgm_read_byte_near(tftBootCommands + addressOffset);
		addressOffset++;
		for (uint8_t i = 0; i < numArgs; i++) {
			commands[i] = pgm_read_byte_near(tftBootCommands + addressOffset++);
		}
		sendCommand(cmd, commands, numArgs);
	}

	uint8_t m = (ILI9341_MADCTL_MX | ILI9341_MADCTL_MY | ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR);
	sendCommand(ILI9341_MADCTL, &m, 1);

	Serial.printf("display initialized\n");
}

void Display::sendCommand(uint8_t cmd, uint8_t *data, uint8_t dataLen) {
	startWrite();
	writeCommand(cmd);
	_spi->writeBytes(data, dataLen); // send the data bytes
	endWrite();
}

void Display::fillRectangleTx(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
	setAddrWindowTx(x, y, w, h);
	for (uint32_t i = 0; i < w*h; i++) {
		_spi->write16(color);
	}
}

void Display::drawVerticalLineTx(int16_t x, int16_t y, int16_t h, uint16_t color) {
	drawLineTx(x, y, x, y + h - 1, color);
}

#ifndef _swap_int16_t
#define _swap_int16_t(a, b)                                                    \
  {                                                                            \
    const int16_t t = a;                                                       \
    a = b;                                                                     \
    b = t;                                                                     \
  }
#endif

void Display::drawLineTx(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
	const boolean steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		_swap_int16_t(x0, y0);
		_swap_int16_t(x1, y1);
	}
	if (x0 > x1) {
		_swap_int16_t(x0, x1);
		_swap_int16_t(y0, y1);
	}

	const int16_t dx = (int16_t) (x1 - x0);
	const int16_t dy = (int16_t) abs(y1 - y0);

	int16_t err = (int16_t) (dx / 2);
	int16_t ystep;

	if (y0 < y1) {
		ystep = 1;
	} else {
		ystep = -1;
	}

	for (; x0 <= x1; x0++) {
		if (steep) {
			drawPixelTx(y0, x0, color);
		} else {
			drawPixelTx(x0, y0, color);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}

void Display::drawPixelTx(int16_t x, int16_t y, uint16_t color) {
	if ((x < 0) || (y < 0)) {
		return;
	}

	if ((x >= ScreenWidth) || (y >= ScreenHeight)) {
		return;
	}
	setAddrWindowTx(x, y, 1, 1);
	_spi->write16(color);
}

void Display::setAddrWindowTx(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h) {
	static uint16_t old_x1 = 0xffff, old_x2 = 0xffff;
	static uint16_t old_y1 = 0xffff, old_y2 = 0xffff;

	const uint16_t x2 = (x1 + w - 1), y2 = (y1 + h - 1);
	if (x1 != old_x1 || x2 != old_x2) {
		writeCommand(ILI9341_CASET); // Column address set
		_spi->write16(x1);
		_spi->write16(x2);
		old_x1 = x1;
		old_x2 = x2;
	}
	if (y1 != old_y1 || y2 != old_y2) {
		writeCommand(ILI9341_PASET); // Row address set
		_spi->write16(y1);
		_spi->write16(y2);
		old_y1 = y1;
		old_y2 = y2;
	}
	writeCommand(ILI9341_RAMWR); // Write to RAM
}

void Display::drawHorizontalLineTx(int16_t x, int16_t y, int16_t w, uint16_t color) {
	drawLineTx(x, y, x + w - 1, y, color);
}
