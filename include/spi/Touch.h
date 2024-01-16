#ifndef Touch_h
#define Touch_h

#include <Arduino.h>
#include <SPI.h>
#include "globals/Pinout.h"
#include "globals/Async.h"

#define SPI_TOUCH_FREQUENCY					2500000
#define MINIMUM_PRESSURE						600
#define MAXIMUM_PRESSURE						3400
#define CFG_EEPROM_TOUCHSCREEN_CALIBRATED 28
#define TOUCHSCREEN_UNTOUCH_COUNT			10 //!< Screen un-press threshold

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
		//uint16_t lastX, lastY;
		tsPoint_t lastTouch;
		tsPoint_t raw, calibrated;
		tsMatrix_t tsMatrix;
		SPIClass* _spi;

		void startRead();
		void endRead();

	public:
		explicit Touch(SPIClass* spi) : _spi(spi) {
		}

		void initialize();
		void poll();

		tsPoint_t getRawTouch();
		tsPoint_t getTouch(uint8_t, uint8_t);

		tsPoint_t getLastTouch();
		boolean isPressed();

		boolean getDisplayPoint(tsPoint_t* calibratedPoint, tsPoint_t* rawPoint);
		boolean setCalibrationMatrix(tsPoint_t* pixelCoord, tsPoint_t* touchCoord);
		boolean readEepromCalibration();
		void writeEepromCalibration();
		void loadHardcodedCalibration();

		void calibrate();

		void clearQueue();

		void debugDrawMatrix();
};

extern Touch* touch;

#endif