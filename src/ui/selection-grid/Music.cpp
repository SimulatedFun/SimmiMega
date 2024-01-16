#include "ui/selection-grid/Music.h"

void MusicSelectionGrid::render() {
	INFO(F("MusicSelectionGrid::render() start"));
	for (uint8_t i = 0; i < tracksPerTab; i++) {
		display->startWrite();
		{
			display->fillRectangleTx(48, y + i * trackHeight, 263, trackHeight - 2, RGB565(0x574b67));
			display->drawDottedRectangleTx(48, y + i * trackHeight, 263, trackHeight - 2, LIGHT_GREY);
		}
		display->endWrite();

		const uint8_t trackId = i + (*currentPage * tracksPerTab);

		Text* text;
		String trackName;
		microSd->begin();
		FileManager::getSongInfoTx(trackId, &trackName);
		microSd->end();
		text = new Text(trackName);
		text->setStyle(1, WHITE);

		if (text->length < 5) {
			delete text;
			char buf[13] = "(track #    ";
			itoa(trackId, &buf[8], 10);
			if (trackId <= 9) {
				buf[9] = ')';
				buf[10] = '\0';
			} else {
				buf[10] = ')';
				buf[11] = '\0';
			}
			text = new Text(buf, 13);
			text->setStyle(1, RGB565(0x919191));
		}

		text->setPosition(48 + text->getPaddingToCenter(262), y + i * trackHeight + 16);
		text->print();
		delete text;

		play->setPosition(0, y + i * trackHeight);
		play->render();
		stop->setPosition(24, y + i * trackHeight);
		stop->render();
	}
	INFO(F("MusicSelectionGrid::render() end"));
}

void MusicSelectionGrid::unPressLastPlay() {
	for (uint8_t j = 0; j < tracksPerTab; j++) {
		const uint8_t trackId = j + (*currentPage * tracksPerTab);
		if (trackId == playingTrackId) {
			INFO("unpress id " << trackId);
			play->setPosition(0, y + j * trackHeight);
			play->depressed = false;
			play->render();
		}
	}
}

void MusicSelectionGrid::handlePress() {
	const tsPoint_t pt = getRelativePoint();
	const uint8_t index = pt.y / trackHeight;
	const uint8_t trackId = index + (*currentPage * tracksPerTab);

	for (uint8_t i = 0; i < tracksPerTab; i++) {
		play->setPosition(0, y + i * trackHeight);
		stop->setPosition(24, y + i * trackHeight);
		if (UIHelper::isWithinTouchBounds(play)) {
			INFO(F("playing track id: ") << trackId);
			unPressLastPlay();
			play->setPosition(0, y + i * trackHeight);
			playingTrackId = trackId;
			play->handlePress();
			INFO("play->handlePress(); done");
			return;
		}
		if (UIHelper::isWithinTouchBounds(stop)) {
			INFO(F("stop playing audio trackId ") << playingTrackId);
			stop->handlePress();
			unPressLastPlay();
			return;
		}
	}

	INFO(F("selected track id: ") << trackId);
	if (callback.function != nullptr) {
		callback.function(trackId);
	}
}