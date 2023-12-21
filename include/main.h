#ifndef main_h
#define main_h

#include <Arduino.h>

#include <SPI.h>
#include "spi/Touch.h"
#include "spi/Display.h"
#include "spi/ExtEeprom.h"
#include "spi/MicroSD.h"

#include "RamBlock.h"
#include "DrawingUtils.h"

#include "states/States.h"
#include "states/MainMenu.h"
#include "states/SpriteEditor.h"
#include "states/Play.h"

void test();
void syncEepromAndRam();

#endif