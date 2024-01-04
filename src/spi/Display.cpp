#include "spi/Display.h"

namespace { // anonymous namespace limits to only use in this file
	inline void swap_int16_t(u16* a, u16* b) {
		u16 const t = *a;
		*a = *b;
		*b = t;
	}
}

void Display::startWrite() {
	_spi->beginTransaction(SPISettings(SPI_TFT_FREQUENCY, MSBFIRST, SPI_MODE0));
	digitalWrite(TFT_CS, LOW);
}

void Display::endWrite() {
	digitalWrite(TFT_CS, HIGH);
	_spi->endTransaction();
}

void Display::writeCommand(u8 cmd) {
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
	u16 addressOffset = 0;
	u8 commands[15];
	while (addressOffset < bootCommandLength) {
		const u8 cmd = pgm_read_byte_near(tftBootCommands + addressOffset);
		addressOffset++;
		const u8 numArgs = pgm_read_byte_near(tftBootCommands + addressOffset);
		addressOffset++;
		for (u8 i = 0; i < numArgs; i++) {
			commands[i] = pgm_read_byte_near(tftBootCommands + addressOffset++);
		}
		sendCommand(cmd, commands, numArgs);
	}

	u8 m = (ILI9341_MADCTL_MX | ILI9341_MADCTL_MY | ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR);
	sendCommand(ILI9341_MADCTL, &m, 1);

	INFO("display initialized");
}

void Display::sendCommand(u8 cmd, u8* data, u8 dataLen) {
	startWrite();
	writeCommand(cmd);
	_spi->writeBytes(data, dataLen); // send the data bytes
	endWrite();
}

void Display::fillRectangleTx(u16 x, u16 y, u16 w, u16 h, u16 color) {
	setAddrWindowTx(x, y, w, h);
	for (uint32_t i = 0; i < w * h; i++) {
		_spi->write16(color);
	}
}

void Display::drawRectangleTx(u16 x, u16 y, u16 w, u16 h, u16 color) {
	drawHorizontalLineTx(x, y, w, color);
	drawHorizontalLineTx(x, y + h - 1, w, color);
	drawVerticalLineTx(x, y, h, color);
	drawVerticalLineTx(x + w - 1, y, h, color);
}

void Display::drawVerticalLineTx(u16 x, u16 y, u16 h, u16 color) {
	drawLineTx(x, y, x, y + h - 1, color);
}

void Display::drawLineTx(u16 x0, u16 y0, u16 x1, u16 y1, u16 color) {
	const boolean steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		swap_int16_t(&x0, &y0);
		swap_int16_t(&x1, &y1);
	}
	if (x0 > x1) {
		swap_int16_t(&x0, &x1);
		swap_int16_t(&y0, &y1);
	}

	const i16 dx = (i16) (x1 - x0);
	const i16 dy = (i16) abs(y1 - y0);

	i16 err = (i16) (dx / 2);
	i16 ystep;

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

void Display::drawPixelTx(u16 x, u16 y, u16 color) {
	if (x < 0 or y < 0) {
		return;
	}
	if (x >= ScreenWidth or y >= ScreenHeight) {
		return;
	}
	setAddrWindowTx(x, y, 1, 1);
	_spi->write16(color);
}

void Display::setAddrWindowTx(u16 x1, u16 y1, u16 w, u16 h) {
	static u16 old_x1 = 0xffff, old_x2 = 0xffff;
	static u16 old_y1 = 0xffff, old_y2 = 0xffff;
	const u16 x2 = (x1 + w - 1), y2 = (y1 + h - 1);
	if (x1 != old_x1 or x2 != old_x2) {
		writeCommand(ILI9341_CASET); // Column address set
		_spi->write16(x1);
		_spi->write16(x2);
		old_x1 = x1;
		old_x2 = x2;
	}
	if (y1 != old_y1 or y2 != old_y2) {
		writeCommand(ILI9341_PASET); // Row address set
		_spi->write16(y1);
		_spi->write16(y2);
		old_y1 = y1;
		old_y2 = y2;
	}
	writeCommand(ILI9341_RAMWR); // Write to RAM
}

void Display::drawHorizontalLineTx(u16 x, u16 y, u16 w, u16 color) {
	drawLineTx(x, y, x + w - 1, y, color);
}

void Display::writeColorTx(u16 color, uint32_t len) {
	if (len == 0) {
		return; // Avoid 0-byte transfers
	}
	while (len-- != 0) {
		_spi->write16(color);
	}
}

void Display::drawDottedRectangleTx(u16 x, u16 y, u16 w, u16 h, u16 color) {
	// vertical lines
	for (u16 i = 0; i < h; i++) {
		if (i % 3 == 0) {
			drawPixelTx(x, y + i, color);
			drawPixelTx(x + w, y + i, color);
		}
	}

	// horizontal lines
	for (u16 i = 0; i < w; i++) {
		if ((i + 1) % 3 == 0) {
			drawPixelTx(x + i, y, color);
			drawPixelTx(x + i, y + h, color);
		}
	}
}
