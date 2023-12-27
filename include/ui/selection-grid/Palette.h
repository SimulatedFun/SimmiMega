#ifndef PaletteSelectionGrid_h
#define PaletteSelectionGrid_h

#include <Arduino.h>
#include "spi/Display.h"
#include "DisplayExtra.h"
#include "ui/UIElement.h"
#include "ui/RoundButton.h"

const uint8_t palettesPerTab = 16;
class PaletteSelectionGrid : public UIElement {
	public:
		uint8_t* currentPage = nullptr;
		void render() override;
		void handlePress() override;
		PaletteSelectionGrid() {
			width = 312;
			height = 180;
		}
		void setPageRef(uint8_t* inPage) {
			currentPage = inPage;
		}
		~PaletteSelectionGrid() {
		}
		Callback<uint8_t> callback;
};

#endif