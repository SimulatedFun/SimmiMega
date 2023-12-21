#ifndef ObjectSelectionGrid_h
#define ObjectSelectionGrid_h

#include <Arduino.h>
#include "spi/Display.h"
#include "DisplayExtra.h"
#include "ui/UIElement.h"
#include "ui/RoundButton.h"

const uint8_t objectsPerTab = 84;
/// ObjectSelectionGrid when the "Choose an Object" screen opens, this is the tray
/// element from which you pick an object from. See ChooseObject namespace
class ObjectSelectionGrid : public UIElement {
	public:
		uint8_t* currentPage = nullptr;
		void render() override;
		void handlePress() override;
		ObjectSelectionGrid() {
			width = 312;
			height = 182;
		}
		void setPageRef(uint8_t* inPage) {
			currentPage = inPage;
		}
		~ObjectSelectionGrid() {
		}
		Callback<unsigned int> callback;
};

#endif