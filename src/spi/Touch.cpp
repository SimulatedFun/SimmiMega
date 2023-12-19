#include "spi/Touch.h"
#include "DrawingUtils.h"

void Touch::initialize() {
	pinMode(TOUCH_CS, OUTPUT);
	digitalWrite(TOUCH_CS, HIGH);
}

void Touch::startRead() {
	_spi->beginTransaction(SPISettings(SPI_TOUCH_FREQUENCY, MSBFIRST, SPI_MODE0));
	digitalWrite(TOUCH_CS, LOW);
}

void Touch::endRead() {
	digitalWrite(TOUCH_CS, HIGH);
	_spi->endTransaction();
}

constexpr uint8_t MaxPointDist = 4;
tsPoint_t Touch::getTouch() {
	// Scan two touch points
	const tsPoint_t pt1 = getRawTouch();
	const tsPoint_t pt2 = getRawTouch();

	// If the points are too far apart, discard
	if (pt1.x - pt2.x < -MaxPointDist || pt1.x - pt2.x > MaxPointDist) {
		return tsPoint_t{0, 0, 0};
	}
	if (pt1.y - pt2.y < -MaxPointDist || pt1.y - pt2.y > MaxPointDist) {
		return tsPoint_t{0, 0, 0};
	}

	// If the two points are relatively close, average them and return the average point
	const uint16_t x = (pt1.x + pt2.x) / 2;
	const uint16_t y = (pt1.y + pt2.y) / 2;
	const uint16_t z = (pt1.z + pt2.z) / 2;
	return tsPoint_t{x, y, z};
}

tsPoint_t Touch::getRawTouch() {
	tsPoint_t point{0, 0, 0};
	startRead();
	{
		// Start YP sample request for x position, read 4 times and keep last sample
		_spi->transfer(0b11010000); // Start new YP conversion
		_spi->transfer(0);			 // Read first 8 bits
		_spi->transfer(0xd0);		 // Read last 8 bits and start new YP conversion
		_spi->transfer(0);			 // Read first 8 bits
		_spi->transfer(0xd0);		 // Read last 8 bits and start new YP conversion
		_spi->transfer(0);			 // Read first 8 bits
		_spi->transfer(0xd0);		 // Read last 8 bits and start new YP conversion

		point.x = _spi->transfer(0); // Read first 8 bits
		point.x = point.x << 5;
		point.x |= 0x1f & (_spi->transfer(0x90) >> 3); // Read last 8 bits and start new XP conversion

		// Start XP sample request for y position, read 4 times and keep last sample
		_spi->transfer(0);	 // Read first 8 bits
		_spi->transfer(0x90); // Read last 8 bits and start new XP conversion
		_spi->transfer(0);	 // Read first 8 bits
		_spi->transfer(0x90); // Read last 8 bits and start new XP conversion
		_spi->transfer(0);	 // Read first 8 bits
		_spi->transfer(0x90); // Read last 8 bits and start new XP conversion

		point.y = _spi->transfer(0); // Read first 8 bits
		point.y = point.y << 5;
		point.y |= 0x1f & (_spi->transfer(0) >> 3); // Read last 8 bits
	}
	endRead();

	startRead();
	{
		// Z sample request
		point.z = 0xFFF;
		_spi->transfer(0xb0);						 // Start new Z1 conversion
		point.z += _spi->transfer16(0xc0) >> 3; // Read Z1 and start Z2 conversion
		point.z -= _spi->transfer16(0x00) >> 3; // Read Z2
	}
	endRead();

	if (point.z == 4095) {
		point.z = 0;
	}

	return point;
}

void Touch::poll() {
	static uint8_t unPressCounter = 0;
	const tsPoint_t point = getTouch();

	// no press detected on the screen
	if (point.z == 0) {
		unPressCounter++;
		if (unPressCounter > TOUCHSCREEN_UNTOUCH_COUNT) {
			unPressCounter = 0;
			touchIsPressed = false;
		}
	}

	// between 0 and 100, we just consider noise and disregard
	if (point.z < MINIMUM_PRESSURE) {
		return;
	}

	unPressCounter = 0;
	raw.x = point.x;
	raw.y = point.y;

	if (getDisplayPoint(&calibrated, &raw)) {
		lastX = calibrated.x;
		lastY = calibrated.y;
		touchIsPressed = true;
		display->fillRectangle(lastX, lastY, 2, 2, BLUE); // debug
	}
}

boolean Touch::getDisplayPoint(tsPoint_t* calibratedPoint, tsPoint_t* rawPoint) {
	if (tsMatrix.Determinant == 0) {
		// DrawingUtils::fill(BLACK);
		// DrawingUtils::simplePrint(0, 10, F("determinant is zero"), RED, 1);
		return false;
	}

	// Operation order is important since we are doing integer math
	// Make sure to add all terms together before dividing, so that the remainder is not rounded off
	// prematurely

	// clang-format off
	calibratedPoint->x = ((tsMatrix.An * rawPoint->x) +
						      (tsMatrix.Bn * rawPoint->y) +
						      tsMatrix.Cn) /
						      tsMatrix.Determinant;

	calibratedPoint->y = ((tsMatrix.Dn * rawPoint->x) +
						      (tsMatrix.En * rawPoint->y) +
						      tsMatrix.Fn) /
						      tsMatrix.Determinant;
	// clang-format on
	return true;
}

///// Reads all calibration data from a specified location in external memory
///// @param location external eeprom memory address to read values
///// @param matrixPtr calibration matrix
///// @return bool whether read was successful
boolean Touch::readEepromCalibration() {
	// RAW(F("======== Calibration Data =======\n"));
	uint8_t isCalibrated = 0;
	eeprom->read(&isCalibrated, CALIB_MEMORY_START + CFG_EEPROM_TOUCHSCREEN_CALIBRATED, 1);
	Serial.printf("isCalibrated: %d\n", isCalibrated);
	if (isCalibrated != 0) {
		for (uint8_t i = 0; i < 7; i++) {
			int32_t data = 0;
			eeprom->read(&data, CALIB_MEMORY_START + i * 4, 4);
			Serial.printf("data: %d %d\n", i, tsMatrix.values[i]);
			tsMatrix.values[i] = data;
			// RAW(tsMatrix.values[i] << " ");
		}
		// RAW(F("\n"));
		// RAW(F("====================================\n\n"));
		GOOD(F("calibration loaded"));
		return true;
	}
	return false;
}

/// Writes all calibration data to a specified location in external memory
/// @param matrixPtr calibration matrix
void Touch::writeEepromCalibration() {
	uint8_t isCalibrated = 0;
	eeprom->write(isCalibrated, CALIB_MEMORY_START + CFG_EEPROM_TOUCHSCREEN_CALIBRATED, 1);
	for (uint8_t i = 0; i < 7; i++) {
		eeprom->write(tsMatrix.values[i], CALIB_MEMORY_START + i * 4, 4);
	}
	isCalibrated = 1;
	eeprom->write(isCalibrated, CALIB_MEMORY_START + CFG_EEPROM_TOUCHSCREEN_CALIBRATED, 1);
}

/// Calculates a calibration matrix given three different display(pixel) points and three different
/// touch points \param lcdPtr pointer to an array of tsPoint_t's that are the display's points
/// \param tftPtr pointer to the array of tsPoint_t's that are the touch values points.
/// \param matrixPtr pointer to the calibration matrix
/// \return whether the matrix was calculated successfully
boolean Touch::setCalibrationMatrix(tsPoint_t* pixelCoord, tsPoint_t* touchCoord) {
	tsMatrix.Determinant =
			  ((touchCoord[0].x - touchCoord[2].x) * (touchCoord[1].y - touchCoord[2].y)) -
			  ((touchCoord[1].x - touchCoord[2].x) * (touchCoord[0].y - touchCoord[2].y));

	if (tsMatrix.Determinant == 0) {
		return false;
	}

	tsMatrix.An = ((pixelCoord[0].x - pixelCoord[2].x) * (touchCoord[1].y - touchCoord[2].y)) -
					  ((pixelCoord[1].x - pixelCoord[2].x) * (touchCoord[0].y - touchCoord[2].y));

	tsMatrix.Bn = ((touchCoord[0].x - touchCoord[2].x) * (pixelCoord[1].x - pixelCoord[2].x)) -
					  ((pixelCoord[0].x - pixelCoord[2].x) * (touchCoord[1].x - touchCoord[2].x));

	tsMatrix.Cn = (touchCoord[2].x * pixelCoord[1].x - touchCoord[1].x * pixelCoord[2].x) *
								 touchCoord[0].y +
					  (touchCoord[0].x * pixelCoord[2].x - touchCoord[2].x * pixelCoord[0].x) *
								 touchCoord[1].y +
					  (touchCoord[1].x * pixelCoord[0].x - touchCoord[0].x * pixelCoord[1].x) *
								 touchCoord[2].y;

	tsMatrix.Dn = ((pixelCoord[0].y - pixelCoord[2].y) * (touchCoord[1].y - touchCoord[2].y)) -
					  ((pixelCoord[1].y - pixelCoord[2].y) * (touchCoord[0].y - touchCoord[2].y));

	tsMatrix.En = ((touchCoord[0].x - touchCoord[2].x) * (pixelCoord[1].y - pixelCoord[2].y)) -
					  ((pixelCoord[0].y - pixelCoord[2].y) * (touchCoord[1].x - touchCoord[2].x));

	tsMatrix.Fn = (touchCoord[2].x * pixelCoord[1].y - touchCoord[1].x * pixelCoord[2].y) *
								 touchCoord[0].y +
					  (touchCoord[0].x * pixelCoord[2].y - touchCoord[2].x * pixelCoord[0].y) *
								 touchCoord[1].y +
					  (touchCoord[1].x * pixelCoord[0].y - touchCoord[0].x * pixelCoord[1].y) *
								 touchCoord[2].y;
	return true;
}

void Touch::loadHardcodedCalibration() {
	tsMatrix.values[0] = -112005;
	tsMatrix.values[1] = 1164;
	tsMatrix.values[2] = 99400964;
	tsMatrix.values[3] = 203;
	tsMatrix.values[4] = -87463;
	tsMatrix.values[5] = 80299539;
	tsMatrix.values[6] = 289678;
	INFO(F("used hardcoded calibration values"));
}

/// Shows a series of points and waits for touch input to calculate a calibration
/// matrix for the points
void Touch::calibrate() {
	Serial.println("calibrating touchscreen");
	// arbitrary points that are decent for creating the transformation matrix
	tsPoint_t pixelPoints[3] = {{32, 32}, {288, 120}, {180, 215}};
	tsPoint_t tsTsPoints[3];

	for (uint8_t i = 0; i < 3; i++) {
		const uint16_t xPos = pixelPoints[i].x;
		const uint16_t yPos = pixelPoints[i].y;

		DrawingUtils::fill(WHITE);
		delay(500);

		display->startWrite();
		{
			Glyphs::drawGlyphTx(xPos - 6, yPos + 11, CALIBRATION_MARKER, RED, 1);
			display->drawHorizontalLineTx(xPos + 3, yPos, ScreenWidth - (xPos + 3), DARK_GREY);
			display->drawHorizontalLineTx(0, yPos, (xPos - 6), DARK_GREY);
			display->drawVerticalLineTx(xPos - 2, 0, yPos - 4, DARK_GREY);
			display->drawVerticalLineTx(xPos - 2, yPos + 5, ScreenHeight - (yPos + 5), DARK_GREY);
		}
		display->endWrite();

		boolean valid = false;
		tsPoint_t p;
		while (!valid) {
			p = getTouch();
			if (p.z > MINIMUM_PRESSURE and p.z < MAXIMUM_PRESSURE and p.x > 0 and p.y > 0) {
				valid = true;
			}
		}
		tsTsPoints[i].x = p.x;
		tsTsPoints[i].y = p.y;
	}

	// Do matrix calculations for calibration and store to external memory
	writeEepromCalibration();
	setCalibrationMatrix(&pixelPoints[0], &tsTsPoints[0]);
}