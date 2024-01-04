#ifndef ChooseObject_h
#define ChooseObject_h

#include <Arduino.h>
#include "spi/Display.h"
#include "ui/selection-grid/Object.h"
// #include "ui/RoundButton.h"
#include "ui/UIHelper.h"
#include "ui/NavigationElement.h"

/// ChooseObject encompasses everything needed for the choose object screen
namespace ChooseObject {
	/// pick() handles drawing UI, waiting for a selection, deallocating, and
	/// returning the selected game object ID. Can also return 0 (no game object ID)
	void pick(boolean includeZerothObject, uint16_t* objectId, boolean* cancelled);

	/// binds to the tray element for selecting a game object from the tray
	void callbackObjectTray(unsigned int selectedObjId);

	/// binds to the exit button, essentially returning the _NO_GAMEOBJECT ID
	void callbackExit(RoundButton& btn);

	/// Binds to the navigational elements at the bottom of the choose obj screen
	void callbackChangeTab(uint8_t num);
	void deallocate();

	void setup();
	void draw();
	void setupUI();
} // namespace ChooseObject

#endif