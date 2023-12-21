#ifndef ChooseDialog_h
#define ChooseDialog_h

#include <Arduino.h>
#include "spi/Display.h"
#include "ui/TrayElement.h"
#include "ui/RoundButton.h"
#include "ui/UIHelper.h"
#include "ui/NavigationElement.h"

/// ChooseFlag encompasses everything needed for the choose flag screen
namespace ChooseDialog {
	/// pick() handles drawing UI, waiting for a selection, deallocating, and
	/// returning the selected flag ID. Can also return 0 (no flag selected)
	uint16_t pick();

	/// binds to the tray element for selecting a flag from the tray
	void callbackDialogTray(unsigned int selectedDialogId);

	/// binds to the exit button, essentially returning the dialog ID
	void callbackExit(RoundButton& btn);

	/// Binds to the navigational elements at the bottom of the choose flag screen
	void callbackChangeTab(uint8_t num);
	void deallocate();

	void setup();
	void draw();
	void setupUI();
} // namespace ChooseFlag

#endif