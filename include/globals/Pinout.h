#ifndef Pinout_h
#define Pinout_h

// Pin Notes
// - Free io pins: 23, 19
// - Default hardware spi pins: sck:14, miso:12, mosi:13, cs:15
// - Default virtual spi pins: sck:18, miso:19, mosi:23, cs:5

#define SHARED_CLK			 14
#define SHARED_MOSI			 13
#define SHARED_MISO			 12
#define TFT_CS					 22
#define TFT_DC					 32
#define TFT_RST				 21
#define TOUCH_CS				 15
#define SD_CS					 4
#define EEPROM_CS				 27
#define I2S_DOUT      23
#define I2S_BCLK      26
#define I2S_LRC       25

// Button input pins
#define PIN_LEFT_BUTTON	 39
#define PIN_RIGHT_BUTTON 35
#define PIN_UP_BUTTON	 36
#define PIN_DOWN_BUTTON	 34

#endif