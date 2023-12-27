#ifndef MusicSelectionGrid_h
#define MusicSelectionGrid_h

#include <Arduino.h>
#include "spi/Display.h"
#include "spi/MicroSD.h"
#include "DisplayExtra.h"
#include "ui/UIElement.h"
#include "ui/RoundButton.h"
#include "FileManager.h"

const uint8_t tracksPerTab = 7;
const uint8_t trackHeight = 24;
class MusicSelectionGrid : public UIElement {
	public:
		RoundButton* play, *stop;
		uint8_t* currentPage = nullptr;
		uint8_t playingTrackId = _NO_MUSIC;
		void render() override;
		void handlePress() override;
		void unPressLastPlay();
		MusicSelectionGrid() {
			width = 312;
			height = 169;
		}
		void setPageRef(uint8_t* inPage) {
			currentPage = inPage;
		}
		~MusicSelectionGrid() {
		}
		Callback<uint8_t> callback;
};

#endif