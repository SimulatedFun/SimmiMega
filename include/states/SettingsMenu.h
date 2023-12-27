#ifndef SettingsMenu_h
#define SettingsMenu_h

#include <Arduino.h>
#include "states/States.h"
#include "ui/SimpleButton.h"
#include "ui/UIHelper.h"
#include "ui/RoundButton.h"
#include "ui/windows/WarningPopup.h"
#include "ui/NavigationElement.h"
#include "ui/ValueButton.h"
#include "ui/TextFieldSelector.h"
#include "ui/windows/ChooseDialog.h"
#include "ui/windows/ChooseRoom.h"
#include "ui/windows/ChooseXY.h"
#include "states/EditTextField.h"
#include "GameSettings.h"

namespace SettingsMenu {
	void setup();
	void setupUI();
	void validateData();
	void loadData();
	void deallocate();
	void draw();
	void loop();

	void callbackExit(RoundButton&);
	void callbackDialog(boolean);
	void callbackChooseRoom(boolean);
	void callbackChooseXY(boolean);

	void callbackEditDirectory(boolean);
	void callbackEditTitle(boolean);
	void callbackEditDescription(boolean);
	void callbackChangeTab(LogicTab);
}

#endif