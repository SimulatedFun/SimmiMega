#ifndef FlagSelectionGrid_h
#define FlagSelectionGrid_h

#include <Arduino.h>
#include "spi/Display.h"
#include "DisplayExtra.h"
#include "ui/UIElement.h"
#include "ui/RoundButton.h"

const uint8_t flagsPerTab = 84;
/// FlagSelectionGrid when the "Choose a Flag" screen opens, this is the tray
/// element from which you pick a flag from. See ChooseFlag namespace
class FlagSelectionGrid : public UIElement {
	public:
		uint8_t* currentPage = nullptr;
		void render() override;
		void handlePress() override;
		FlagSelectionGrid() {
			width = 312;
			height = 182;
		}
		void setPageRef(uint8_t* inPage) {
			currentPage = inPage;
		}
		~FlagSelectionGrid() {
		}
		Callback<unsigned int> callback;
};

#endif