#ifndef Touch_h
#define Touch_h

#include <Arduino.h>
#include <SPI.h>
#include "globals/Pinout.h"

#define SPI_TOUCH_FREQUENCY 2500000

// region Touchscreen Config
#define MINIMUM_PRESSURE						100
#define MAXIMUM_PRESSURE						1000
#define CFG_EEPROM_TOUCHSCREEN_CAL_AN		0
#define CFG_EEPROM_TOUCHSCREEN_CAL_BN		4
#define CFG_EEPROM_TOUCHSCREEN_CAL_CN		8
#define CFG_EEPROM_TOUCHSCREEN_CAL_DN		12
#define CFG_EEPROM_TOUCHSCREEN_CAL_EN		16
#define CFG_EEPROM_TOUCHSCREEN_CAL_FN		20
#define CFG_EEPROM_TOUCHSCREEN_CAL_DV		24
#define CFG_EEPROM_TOUCHSCREEN_CALIBRATED 28

// How many values needed to be considered un-pressing the screen
#define TOUCHSCREEN_UNTOUCH_COUNT 10
// endregion

// Touchscreen point
struct tsPoint_t {
		uint16_t x, y, z;
};

// Calibration matrix
union tsMatrix_t {
		int32_t values[7];
		struct {
				int32_t An, Bn, Cn, Dn, En, Fn, Determinant;
		};
};

class Touch {
	private:
		boolean touchIsPressed = false;
		uint16_t lastX, lastY;
		tsPoint_t raw, calibrated;
		tsMatrix_t tsMatrix;
		SPIClass* _spi;
		void startRead();
		void endRead();

	public:
		explicit Touch(SPIClass* spi) : _spi(spi) {
		}

		void initialize();
		tsPoint_t getRawTouch();

		uint16_t touchX() {
			return lastX;
		}

		uint16_t touchY() {
			return lastY;
		}

		boolean isPressed() {
			return touchIsPressed;
		}

		boolean getDisplayPoint(tsPoint_t* calibratedPoint, tsPoint_t* rawPoint);
		boolean setCalibrationMatrix(tsPoint_t* pixelCoord, tsPoint_t* touchCoord);
		boolean readEepromCalibration();
		void writeEepromCalibration();
		void loadHardcodedCalibration();
		void calibrate();
		void poll();
};

#endif