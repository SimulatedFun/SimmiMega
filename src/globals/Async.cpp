#include "globals/Async.h"

uint32_t lastCall[AsyncIdCount] = {0};

boolean checkTimer(uint16_t timeMs, uint8_t timerId, boolean autoReset) {
	if (timeMs + lastCall[timerId] <= millis()) {
		if (autoReset) {
			resetTimer(timerId);
		}
		return true;
	}
	return false;
}

void resetTimer(uint8_t timerId) {
	lastCall[timerId] = millis();
}
