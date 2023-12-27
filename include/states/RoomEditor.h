#ifndef RoomEditorState_h
#define RoomEditorState_h

#include <Arduino.h>
#include "ui/UIElement.h"
#include "globals/Async.h"
#include "RoomHelper.h"
#include "globals/DataBounds.h"
#include "states/States.h"
#include "DisplayExtra.h"
#include "ui/RoundButton.h"
#include "DrawBMP.h"
#include "ui/ValueButton.h"
#include "ui/windows/ChoosePalette.h"
#include "ui/windows/ChooseRoom.h"
#include "ui/windows/ChooseObject.h"
#include "ui/windows/ChooseMusic.h"

namespace RoomEditor {
	void setup();
	void setupUI();
	void loop();
	void draw();

	void deallocate();

	void callbackExit(RoundButton&);
	void callbackChoosePencil(RoundButton&);
	void callbackChooseEraser(RoundButton&);
	void callbackClear(RoundButton&);
	void callbackPlaceObject(uint8_t);
	void callbackSelectTray(boolean);
	void callbackChangeTab(uint8_t);
	void callbackChoosePlayer(boolean);
	void callbackChoosePalette(boolean);
	void callbackMusic(boolean);

	void loopMarchingAnts();

#define MARCHING_ANTS_WIDTH  15
#define MARCHING_ANTS_HEIGHT 15

} // namespace RoomEditor

#endif
