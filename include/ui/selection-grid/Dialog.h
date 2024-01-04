#ifndef DialogSelectionGrid_h
#define DialogSelectionGrid_h

#include <Arduino.h>
#include "spi/Display.h"
#include "DisplayExtra.h"
#include "ui/UIElement.h"
#include "ui/RoundButton.h"

const uint8_t dialogsPerTab = 8;
class DialogSelectionGrid : public UIElement {
	public:
		uint8_t* currentPage = nullptr;
        boolean showZeroth = false;
		void render() override;
		void handlePress() override;
		DialogSelectionGrid() {
			width = 312;
			height = 169;
		}
		void setPageRef(uint8_t* inPage) {
			currentPage = inPage;
		}
		~DialogSelectionGrid() {
		}
		Callback<unsigned int> callback;
};

#endif