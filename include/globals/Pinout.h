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
#define SD_CS					 5
#define EEPROM_CS				 27
#define AUDIO_DIN				 33
#define AUDIO_BLCK			 26
#define AUDIO_LRCLK			 25

// Button input pins
#define PIN_BUTTON_INPUT 19
#define PIN_LEFT_BUTTON	 19
#define PIN_RIGHT_BUTTON 19
#define PIN_UP_BUTTON	 19
#define PIN_DOWN_BUTTON	 19

#endif