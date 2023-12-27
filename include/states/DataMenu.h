#ifndef DataMenu_h
#define DataMenu_h

#include <Arduino.h>
#include "states/States.h"
#include "ui/SimpleButton.h"
#include "ui/UIHelper.h"
#include "ui/RoundButton.h"
#include "ui/windows/WarningPopup.h"
#include "Dialog.h"
#include "GameSettings.h"
#include "RoomHelper.h"

namespace DataMenu {
	void setup();
	void setupUI();
	void deallocate();
	void draw();
	void drawTextBox(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
	void loop();

	void callbackExit(RoundButton&);
	void callbackNewProject(boolean);
	void callbackSaveProject(boolean);
	void callbackListGames(boolean);

	sd_err setOrMakeGameDirectory();
	sd_err saveSettingsToFile();
	sd_err saveGameObjectsToFile();
	sd_err saveRoomsToFile();
	sd_err savePalettesToFile();
	sd_err saveDialogToFile();

	boolean checkIfError(sd_err errCode);
} // namespace DataMenu


#endif