#ifndef System_h
#define System_h

#include <Arduino.h>
#include "globals/DataBounds.h"
#include "globals/SerialDebugging.h"
#include "spi/ExtEeprom.h"
#include "RamBlock.h"

namespace System {
	void syncEepromAndRam();
}

#endif