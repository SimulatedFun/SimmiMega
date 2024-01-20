#ifndef ChooseMusic_h
#define ChooseMusic_h

#include <Arduino.h>
#include "spi/Display.h"
#include "spi/MicroSD.h"
#include "Palette.h"
#include "Sound.h"
#include "ui/TrayElement.h"
#include "ui/RoundButton.h"
#include "ui/UIHelper.h"
#include "ui/NavigationElement.h"
#include "ui/selection-grid/Music.h"
#include "FileManager.h"

namespace ChooseMusic {
	void setup();
	void draw();
	void setupUI();

	uint8_t pick();
	void deallocate();

	void callbackExit(RoundButton&);
	void callbackChangeTab(uint8_t number);
	void callbackMusicTray(uint8_t selectedTrackId);
	void callbackPlay(RoundButton&);
	void callbackStop(RoundButton&);
}

#endif