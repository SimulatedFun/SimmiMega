/// While you can program asynchronous operations using interrupt pins or interrupt timers, I'm
/// mainly using a simple check method inside the main loop to check against a self-implemented
/// timer.
#ifndef Async_h
#define Async_h

#include <Arduino.h>

boolean checkTimer(uint16_t ms, uint8_t id, boolean autoReset = false);
void resetTimer(uint8_t id);

enum AsyncId {
	// Waiting between touchscreen presses (debounce)
	BetweenTouches,

	// Waiting between changing screens and checking for touch input
	ScreenChangeWait,

	// Let asyncIdCount be the last enum value to track the number of timers
	AsyncIdCount
};

#endif
