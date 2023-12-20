#include "globals/Async.h"

uint32_t lastCall[AsyncIdCount] = {0};

/// Asynchronous timer
/// \param ms amount of time in milliseconds since last timer reset
/// \param timerId which timer to check
/// \return if there has been ms time since last timer reset
boolean checkTimer(uint16_t ms, uint8_t id, boolean autoReset) { // remember to reset when condition is true!!
	if (ms + lastCall[id] <= millis()) {
		if (autoReset) {
			resetTimer(id);
		}
		return true;
	}
	return false;
}

/// Resets an async timer by timerId
/// \param id which timer to reset
void resetTimer(uint8_t id) {
	lastCall[id] = millis();
}
