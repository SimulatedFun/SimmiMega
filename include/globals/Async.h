/// While you can program asynchronous operations using interrupt pins or interrupt timers, I'm
/// mainly using a simple check method inside the main loop to check against a self-implemented
/// timer.
#ifndef Async_h
#define Async_h

#include <Arduino.h>

/// Asynchronous timer
/// \param timeMs amount of time in milliseconds since last timer reset
/// \param timerId which timer to check
/// \param autoReset if the timer should auto reset once it has gone off
/// \return if there has been ms time since last timer reset
/// \note Remember to reset after condition is true if autoReset is turned off
boolean checkTimer(uint16_t timeMs, uint8_t timerId, boolean autoReset = false);

/// Resets an async timer by timerId
/// \param timerId which timer to reset
void resetTimer(uint8_t timerId);

enum TimerId : uint8_t {
	// Waiting between touchscreen presses (debounce)
	BetweenTouches,

	// Waiting between changing screens and checking for touch input
	ScreenChangeWait,

	// Waiting at least x amount of seconds between play mode frame changes
	AsyncPlayModeAnimation,

	// Waiting at least x amount of seconds between button presses
	ButtonMoveDelay,


	// For the up and down arrow in the bottom right of the dialog box in play mode
	DialogArrowAnimation,

	// For the marching ants animation in the room editor
	MarchingAntsAnimation,
	DialogCursorFlash,

	// Play mode dynamic objects
	PathfindingDelay,
	WanderingDelay,

	// Let asyncIdCount be the last enum value to track the number of timers
	AsyncIdCount
};

#endif
