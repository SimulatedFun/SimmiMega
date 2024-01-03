#include "Commands.h"

const int MAX_CMD_LENGTH = 30;
char cmd[MAX_CMD_LENGTH];

void Commands::check() {
	if (Serial.available() == 0) {
		return;
	}

	// clear the old values each time
	for (uint8_t i = MAX_CMD_LENGTH; i < MAX_CMD_LENGTH; i++) {
		cmd[i] = '\0';
	}

	Serial.readBytesUntil('\n', cmd, MAX_CMD_LENGTH - 1);
	cmd[MAX_CMD_LENGTH - 1] = '\0';
	Serial.printf("Command: %s\n", cmd);

	INFO(F("read command: ") << cmd);

	if (strncmp(cmd, "help", 4) == 0) {
		help();
		return;
	}

	if (strncmp(cmd, "ram", 3) == 0) {
		Serial.printf("Free memory: %d", esp_get_minimum_free_heap_size());
		return;
	}

	// manually recalibrate the touch screen
	if (strncmp(cmd, "calibrate", 9) == 0) {
		touch->calibrate();
		return;
	}

	// show the debug font screen for 5s
	if (strncmp(cmd, "font", 4) == 0) {
		Glyphs::debugFont();
		return;
	}

	if (strncmp(cmd, "ls", 2) == 0) {
		microSd->begin();
		microSd->listDirTx("/", 10);
		microSd->end();
		return;
	}

	if (strncmp(cmd, "cat", 3) == 0) {
		microSd->begin();
		microSd->readFile(&cmd[4]);
		microSd->end();
		return;
	}
}

void Commands::help() {
	RAW(F("\n=== SimmiMega Help ===\nAvailable commands:\n"));
	RAW(F("sdloop - yield until sd detected\n"));
	RAW(F("reinit - reinit sd\n"));
	RAW(F("draw \"filename\" - draws a bitmap image\n"));
	RAW(F("play \"filename\" - plays a wav file (needs manual Audio::buffer() to fill buffers)\n"));
	RAW(F("ping - prints \"pong\"\n"));
	RAW(F("ram - prints available SRAM\n"));
	RAW(F("ls - lists all files/directories on sd card\n"));
	RAW(F("cat \"filename\" - prints contents of a file\n"));
	RAW(F("rm \"filename\" - deletes a file\n"));
	RAW(F("calibrate - calibrates the touch screen\n"));
	RAW(F("font - draws every glyph on screen\n"));
	// RAW(F("glyph \"glyph id\" - 'draws' a glyph to console\n"));
	RAW(F("obj \"gameobject id\" - 'draws' a gameobject's first frame to console\n"));
	RAW(F("gameobj \"gameobject id\" - prints out a gameobj's toString to console\n"));
	RAW(F("reset \"gameobject id\" - resets a gameobj's logic\n"));
	RAW(F("room \"room id\" - prints out every object in a room\n"));
	RAW(F("menu - re-enters menu state\n"));
	RAW(F("thumbnail - save room thumbnails for caching\n"));
	RAW(F("settings - list current game settings\n"));
	RAW(F("restart - soft restart device\n"));
}