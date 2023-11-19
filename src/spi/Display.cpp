#include "spi/Display.h"

Arduino_ESP32SPI bus = Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCK, TFT_MOSI, TFT_MISO);
Arduino_ILI9341 display = Arduino_ILI9341(&bus, TFT_RESET);

void Display::initialize() {
	display.begin();
	display.setRotation(1);
	Display::fillRectangle(312, 0, 8, 240, BLACK); // rightmost black edge
	ESP_LOGV(Info, "display initialized");
}

void Display::startWrite() {
	display.startWrite();
}

void Display::endWrite() {
	display.endWrite();
}

void Display::drawPixelTx(uint16_t x, uint16_t y, uint16_t color) {
	display.writePixel((int16_t) x, (int16_t) y, color);
}

void Display::drawHorizontalLineTx(uint16_t x, uint16_t y, uint16_t w, uint16_t color) {
	display.writeFastHLine((int16_t) x, (int16_t) y, (int16_t) w, color);
}

void Display::drawVerticalLineTx(uint16_t x, uint16_t y, uint16_t h, uint16_t color) {
	display.writeFastVLine((int16_t) x, (int16_t) y, (int16_t) h, color);
}

void Display::fillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
	display.fillRect((int16_t) x, (int16_t) y, (int16_t) w, (int16_t) h, color);
}

void Display::fillRectangleTx(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
	display.writeFillRect((int16_t) x, (int16_t) y, (int16_t) w, (int16_t) h, color);
}

void Display::setAddrWindow(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h) {
	display.writeAddrWindow((int16_t) x1, (int16_t) y1, w, h);
}

void Display::drawDottedRectangleTx(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
												uint16_t color) {
	// vertical lines
	for (uint16_t i = 0; i < h; i++) {
		if (i % 3 == 0) {
			drawPixelTx(x, y + i, color);
			drawPixelTx(x + w, y + i, color);
		}
	}

	// horizontal lines
	for (uint16_t i = 0; i < w; i++) {
		if ((i + 1) % 3 == 0) {
			drawPixelTx(x + i, y, color);
			drawPixelTx(x + i, y + h, color);
		}
	}
}

void Display::drawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
	startWrite();
	drawRectangleTx(x, y, w, h, color);
	endWrite();
}

void Display::drawRectangleTx(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
	drawHorizontalLineTx(x, y, w, color);
	drawHorizontalLineTx(x, y + h - 1, w, color);
	drawVerticalLineTx(x, y, h, color);
	drawVerticalLineTx(x + w - 1, y, h, color);
}

void Display::drawDottedRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
	startWrite();
	drawDottedRectangleTx(x, y, w, h, color);
	endWrite();
}

void Display::writeColor(uint16_t color, uint32_t len) {
	display.writeRepeat(color, len);
}
