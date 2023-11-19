#ifndef main_h
#define main_h

#include <Arduino.h>
#include "globals/StateMachine.h"
#include "globals/DataBounds.h"
#include "globals/Glyphs.h"
#include "globals/SerialDebugging.h"

#include "spi/Display.h"
#include "spi/MicroSD.h"
#include "spi/ExtEeprom.h"
#include "RamBlock.h"

void debugExternalMemory();

#endif