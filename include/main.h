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
#include "Bitmaps/Intro.h"
#include "FileManager.h"
#include "Commands.h"
#include "System.h"

#include "states/States.h"
#include "states/MainMenu.h"
#include "states/SpriteEditor.h"
#include "states/Play.h"
#include "states/LogicEditor.h"
#include "states/RoomEditor.h"
#include "states/DialogEditor.h"
#include "states/PaletteEditor.h"
#include "states/SettingsMenu.h"
#include "states/DataMenu.h"
#include "states/SavedGames.h"

void test();

#endif