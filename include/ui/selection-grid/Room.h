#ifndef RoomSelectionGrid_h
#define RoomSelectionGrid_h

#include <Arduino.h>
#include "spi/Display.h"
#include "spi/MicroSD.h"

#include "globals/DataBounds.h"
#include "globals/Bitwise.h"

#include "ui/UIElement.h"
#include "Palette.h"
#include "RoomHelper.h"
#include "FileManager.h"

class RoomSelectionGrid : public UIElement {
	public:
		void render() override;
		void handlePress() override;
		RoomSelectionGrid() {
			width = 312;
			height = 240;
		}
		~RoomSelectionGrid() override {}
		Callback<unsigned int> callback;
};

#endif