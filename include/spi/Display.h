// Register and boot commands adapted from https://github.com/adafruit/Adafruit_ILI9341

#ifndef Display_H
#define Display_H

#include <Arduino.h>
#include <SPI.h>
#include "globals/Pinout.h"
#include "globals/Colors.h"

#define ILI9341_RST_DELAY	  150 ///< delay ms wait for reset finish
#define ILI9341_SLPIN_DELAY  150 ///< delay ms wait for sleep in finish
#define ILI9341_SLPOUT_DELAY 150 ///< delay ms wait for sleep out finish

// region ILI9341 Registers
#define ILI9341_NOP		0x00 ///< No-op register
#define ILI9341_SWRESET 0x01 ///< Software reset register
#define ILI9341_RDDID	0x04 ///< Read display identification information
#define ILI9341_RDDST	0x09 ///< Read Display Status

#define ILI9341_SLPIN  0x10 ///< Enter Sleep Mode
#define ILI9341_SLPOUT 0x11 ///< Sleep Out
#define ILI9341_PTLON  0x12 ///< Partial Mode ON
#define ILI9341_NORON  0x13 ///< Normal Display Mode ON

#define ILI9341_RDMODE		0x0A ///< Read Display Power Mode
#define ILI9341_RDMADCTL	0x0B ///< Read Display MADCTL
#define ILI9341_RDPIXFMT	0x0C ///< Read Display Pixel Format
#define ILI9341_RDIMGFMT	0x0D ///< Read Display Image Format
#define ILI9341_RDSELFDIAG 0x0F ///< Read Display Self-Diagnostic Result

#define ILI9341_INVOFF	 0x20 ///< Display Inversion OFF
#define ILI9341_INVON	 0x21 ///< Display Inversion ON
#define ILI9341_GAMMASET 0x26 ///< Gamma Set
#define ILI9341_DISPOFF	 0x28 ///< Display OFF
#define ILI9341_DISPON	 0x29 ///< Display ON

#define ILI9341_CASET 0x2A ///< Column Address Set
#define ILI9341_PASET 0x2B ///< Page Address Set
#define ILI9341_RAMWR 0x2C ///< Memory Write
#define ILI9341_RGBSET 0x2D ///< Color set
#define ILI9341_RAMRD 0x2E ///< Memory Read

#define ILI9341_PTLAR	 0x30 ///< Partial Area
#define ILI9341_VSCRDEF	 0x33 ///< Vertical Scrolling Definition
#define ILI9341_MADCTL	 0x36 ///< Memory Access Control
#define ILI9341_VSCRSADD 0x37 ///< Vertical Scrolling Start Address
#define ILI9341_PIXFMT	 0x3A ///< COLMOD: Pixel Format Set

#define ILI9341_FRMCTR1 0xB1 ///< Frame Rate Control (In Normal Mode/Full Colors)
#define ILI9341_FRMCTR2 0xB2 ///< Frame Rate Control (In Idle Mode/8 colors)
#define ILI9341_FRMCTR3 0xB3 ///< Frame Rate control (In Partial Mode/Full Colors)
#define ILI9341_INVCTR	0xB4 ///< Display Inversion Control
#define ILI9341_DFUNCTR 0xB6 ///< Display Function Control

#define ILI9341_PWCTR1 0xC0 ///< Power Control 1
#define ILI9341_PWCTR2 0xC1 ///< Power Control 2
#define ILI9341_PWCTR3 0xC2 ///< Power Control 3
#define ILI9341_PWCTR4 0xC3 ///< Power Control 4
#define ILI9341_PWCTR5 0xC4 ///< Power Control 5
#define ILI9341_VMCTR1 0xC5 ///< VCOM Control 1
#define ILI9341_VMCTR2 0xC7 ///< VCOM Control 2

#define ILI9341_RDID1 0xDA ///< Read ID 1
#define ILI9341_RDID2 0xDB ///< Read ID 2
#define ILI9341_RDID3 0xDC ///< Read ID 3
#define ILI9341_RDID4 0xDD ///< Read ID 4

#define ILI9341_GMCTRP1	  0xE0 ///< Positive Gamma Correction
#define ILI9341_GMCTRN1	  0xE1 ///< Negative Gamma Correction
#define ILI9341_DGMCTRN1  0xE2 ///< Digital Gamma Control 1
#define ILI9341_DGMCTRN2  0xE3 ///< Digital Gamma Control 2
#define ILI9341_INTCTR	  0xF6 ///< Interface Control
#define ILI9341_PWRCTRA	  0xCB ///< Power control A
#define ILI9341_PWRCTRB	  0xCF ///< Power control B
#define ILI9341_DRVTIMA1  0xE8 ///< Driver timing control A (1)
// E9 Driver timing control A (2)
#define ILI9341_DRVTIMB	  0xEA ///< Driver timing control B
#define ILI9341_PWRONSEQ  0xED ///< Power on sequence control
#define ILI9341_3GAMMA	  0xF2 ///< Enable 3G
#define ILI9341_PUMPRATIO 0xF7 ///< Pump ratio control

#define ILI9341_MADCTL_MY	0x80 ///< Bottom to top
#define ILI9341_MADCTL_MX	0x40 ///< Right to left
#define ILI9341_MADCTL_MV	0x20 ///< Reverse Mode
#define ILI9341_MADCTL_ML	0x10 ///< LCD refresh Bottom to top
#define ILI9341_MADCTL_RGB 0x00 ///< Red-Green-Blue pixel order
#define ILI9341_MADCTL_BGR 0x08 ///< Blue-Green-Red pixel order
#define ILI9341_MADCTL_MH	0x04 ///< LCD refresh right to left
// endregion

// region Boot command format: <cmd> <argument count> <arg 1> <arg 2> <...>
// clang-format off
static const PROGMEM uint8_t tftBootCommands[] = {
	0xEF, 3, 0x03, 0x80, 0x02,
	ILI9341_PWRCTRB, 3, 0x00, 0xC1, 0x30,
	ILI9341_PWRONSEQ, 4, 0x64, 0x03, 0x12, 0x81,
	ILI9341_DRVTIMA1, 3, 0x85, 0x00, 0x78,
	ILI9341_PWRCTRA, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
	ILI9341_PUMPRATIO, 1, 0x20, // Pump ratio control
	ILI9341_DRVTIMB, 2, 0x00, 0x00, // Driver timing control B
	ILI9341_PWCTR1, 1, 0x23, // Power control VRH[5:0]
	ILI9341_PWCTR2, 1, 0x10, // Power control SAP[2:0];BT[3:0]
	ILI9341_VMCTR1, 2, 0x3e, 0x28, // VCM control
	ILI9341_VMCTR2, 1, 0x86, // VCM control2
	ILI9341_MADCTL, 1, 0x48, // Memory Access Control
	ILI9341_VSCRSADD, 1, 0x00, // Vertical scroll zero
	ILI9341_PIXFMT, 1, 0x55,
	ILI9341_FRMCTR1, 2, 0x00, 0x10, // Max frame-rate to reduce flicker
	ILI9341_DFUNCTR, 3, 0x08, 0x82, 0x27,
	ILI9341_3GAMMA, 1, 0x00, // 3Gamma Function Disable
	ILI9341_GAMMASET, 1, 0x01, // Gamma curve selected
	ILI9341_GMCTRP1, 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, // Set Gamma
	0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
	ILI9341_GMCTRN1, 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, // Set Gamma
	0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
	ILI9341_SLPOUT, 1, 0x80, // Exit Sleep
	ILI9341_DISPON, 1, 0x80, // Display on
};
constexpr uint8_t bootCommandLength = sizeof(tftBootCommands);
// clang-format on
// endregion

#define SPI_TFT_FREQUENCY 120000000
constexpr uint16_t ScreenWidth = 320;
constexpr uint16_t ScreenHeight = 240;

class Display {
	private:
		SPIClass* _spi;
		void sendCommand(uint8_t commandByte, uint8_t* dataBytes = nullptr, uint8_t numDataBytes = 0);
		void writeCommand(uint8_t cmd);

	public:
		explicit Display(SPIClass* spi) : _spi(spi) {}
		void initialize();

		void startWrite();
		void endWrite();

		void fillRectangleTx(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
		void fillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
			startWrite();
			fillRectangleTx(x, y, w, h, color);
			endWrite();
		}

		void drawRectangleTx(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
		void drawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
			startWrite();
			drawRectangleTx(x, y, w, h, color);
			endWrite();
		}

		void drawDottedRectangleTx(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
		void drawDottedRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
			startWrite();
			drawDottedRectangleTx(x, y, w, h, color);
			endWrite();
		}

		void drawLineTx(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
		void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
			startWrite();
			drawLineTx(x0, y0, x1, y1, color);
			endWrite();
		}

		void drawHorizontalLineTx(int16_t x, int16_t y, int16_t w, uint16_t color);
		void drawHorizontalLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
			startWrite();
			drawHorizontalLineTx(x, y, w, color);
			endWrite();
		}

		void drawVerticalLineTx(int16_t x, int16_t y, int16_t h, uint16_t color);
		void drawVerticalLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
			startWrite();
			drawVerticalLineTx(x, y, h, color);
			endWrite();
		}

		void drawPixelTx(int16_t x, int16_t y, uint16_t color);
		void drawPixel(int16_t x, int16_t y, uint16_t color) {
			startWrite();
			drawPixelTx(x, y, color);
			endWrite();
		}

		void setAddrWindowTx(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h);
		void writeColorTx(uint16_t color, uint32_t len);
};

extern Display* display;

#endif