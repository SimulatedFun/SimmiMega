#include "Sound.h"

Audio audioLib;

void Sound::initialize() {
	audioLib.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
	audioLib.setVolume(14);
	audioLib.setBufsize(1600*10*2, 65535*10*2);
}

void Sound::playFile(const String& filename) {
	INFO("Sound::playFile: " << filename << " (start)");
	//microSd->begin();
	audioLib.connecttoFS(SD, filename.c_str());
	audioLib.setFileLoop(true);
	//microSd->end();
	INFO("Sound::playFile: " << filename << " (end)");
}

void Sound::stop() {
	audioLib.stopSong();
}

void Sound::buffer() {
	//INFO("Sound::buffer");
	//microSd->begin();
	audioLib.loop();
	//microSd->end();
}