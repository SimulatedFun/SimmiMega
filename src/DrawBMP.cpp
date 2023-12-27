#include "DrawBMP.h"

// This function opens a BMP file and displays it at the given coordinates. It's
// sped up by reading many pixels worth of data at a time (rather than pixel by
// pixel). Increasing the buffer size takes more of the Arduino's precious RAM
// but makes loading a little faster. 20 pixels is a good balance.

// Code from https://github.com/adafruit/Adafruit_HX8357_Library/blob/master/examples/spitftbitmap/spitftbitmap.ino

def u8 PIXEL_BUFFER_SIZE = 60;

void drawRGBBitmap(u16 x, u16 y, const u16 *bitmap, u16 w, u16 h, uint16_t transparentColor) {
	display->startWrite();
	for (i16 j = 0; j < h; j++, y++) {
		for (i16 i = 0; i < w; i++) {
			if (bitmap[j * w + i] == transparentColor) {
				continue;
			}
			display->drawPixelTx(x + i, y, bitmap[j * w + i]);
		}
	}
	display->endWrite();
}

/// Draws a BMP image to the screen at the given coordinates
/// \param filename Name of the file to load
/// \param x The x coordinate of the top left corner of the image
/// \param y The y coordinate of the top left corner of the image
/// \return True if the image was drawn successfully
sd_err bmpDraw(str filename, u16 x, u16 y, u16 maxWidth, u16 maxHeight) {
	microSd->begin();

	// Open requested file on SD card
	File bmpFile = SD.open(filename);
	if (!bmpFile) {
		ERROR(F("File not found"));
		microSd->end();
		return errFailOpenFile;
	}

	// BMP Identifier
	if (read16(bmpFile) != 0x4D42) {
		ERROR(F("Invalid BMP signature"));
		bmpFile.close();
		microSd->end();
		return errFileInvalid;
	}

	const u32 fileSize = read32(bmpFile);

	read32(bmpFile); // unused/reserved bytes

	const u32 bmpImageOffset = read32(bmpFile); // Offset to image data in bytes from beginning of file);

	const u32 headerSize = read32(bmpFile); // Read DIB header

	const u16 bmpWidth = read32(bmpFile);
	if (bmpWidth > maxWidth) {
		microSd->end();
		return errFileInvalid;
	}

	const u16 bmpHeight = read32(bmpFile);
	if (bmpHeight > maxHeight) {
		microSd->end();
		return errFileInvalid;
	}

	// # planes -- must be '1'
	if (read16(bmpFile) != 1) {
		ERROR(F("Number of planes != 1"));
		bmpFile.close();
		microSd->end();
		return errFileInvalid;
	}

	// Pixel color depth (currently must be 24)
	const u8 bmpDepth = read16(bmpFile); // bits per pixel
	if (bmpDepth != 24) {
		ERROR(F("Bit depth not 24"));
		bmpFile.close();
		microSd->end();
		return errFileInvalid;
	}

	if (read32(bmpFile) != 0) { // 0 = uncompressed
		ERROR(F("Compression not supported"));
		bmpFile.close();
		microSd->end();
		return errFileInvalid;
	}

	// BMP rows are padded (if needed) to 4-byte boundary
	const u32 rowSize = (bmpWidth * 3 + 3) & ~3; // rowSize not always = bmpWidth

	// Crop area to be loaded
	u16 w = bmpWidth;
	u16 h = bmpHeight;
	if ((x + w - 1) >= ScreenWidth) {
		w = ScreenWidth - x;
	}
	if ((y + h - 1) >= ScreenWidth) {
		h = ScreenWidth - y;
	}

	// Set TFT address window to clipped image bounds
	display->startWrite();
	{
		display->setAddrWindowTx(x, y, w, h);

		u32 pos = 0;
		u8* buffer = new u8[PIXEL_BUFFER_SIZE]; // pixel buffer (R+G+B per pixel)
		u8 bufferIterator = PIXEL_BUFFER_SIZE;		  // Current position in buffer

		for (u16 row = 0; row < h; row++) {
			pos = bmpImageOffset + (bmpHeight - 1 - row) * rowSize;
			if (bmpFile.position() != pos) { // Need seek?
				display->endWrite();
				bmpFile.seek(pos);
				bufferIterator = PIXEL_BUFFER_SIZE; // Force buffer reload
			}

			for (u16 col = 0; col < w; col++) { // For each pixel...
				// Time to read more pixel data?
				if (bufferIterator >= PIXEL_BUFFER_SIZE) { // Indeed
					bmpFile.read(buffer, PIXEL_BUFFER_SIZE);
					bufferIterator = 0; // Set index to beginning
					display->startWrite();
				}

				// Convert pixel from BMP to TFT format, push to display
				const u8 b = buffer[bufferIterator++];
				const u8 g = buffer[bufferIterator++];
				const u8 r = buffer[bufferIterator++];
				const u16 color = RGB565(r, g, b);
				display->writeColorTx(color, 1);
			} // end pixel
		}	  // end scanline
		delete[] buffer;
	}
	display->endWrite();

	bmpFile.close();
	microSd->end();
	return noSdError;
}

/// Reads two bytes from a file and returns them as a u16
/// Seeks the file forward two bytes as well
/// \param f File to read from
/// \return Next two bytes in the file
u16 read16(File& f) {
	u16 result = 0;
	((u8*) &result)[0] = f.read(); // LSB
	((u8*) &result)[1] = f.read(); // MSB
	return result;
}

/// Reads four bytes from a file and returns them as a u32
/// Seeks the file forward four bytes as well
/// \param f File to read from
/// \return Next four bytes in the file
u32 read32(File& f) {
	u32 result = 0;
	((u8*) &result)[0] = f.read(); // LSB
	((u8*) &result)[1] = f.read();
	((u8*) &result)[2] = f.read();
	((u8*) &result)[3] = f.read(); // MSB
	return result;
}