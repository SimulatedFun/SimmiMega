#include "Commands.h"

void Commands::check() {
	if (!Serial.available()) {
		return;
	}

	const String cmd = Serial.readString();
	Serial.printf("Command: %s\n", cmd.c_str());
	delay(4);

	if (strcmp(cmd.c_str(), "help") == 0) {
		help();
		return;
	}

	if (strcmp(cmd.c_str(), "ram") == 0) {
		Serial.printf("Free memory: %d", esp_get_minimum_free_heap_size());
		return;
	}

	// manually recalibrate the touch screen
	if (strcmp(cmd.c_str(), "calibrate") == 0) {
		touch->calibrate();
		return;
	}

	// show the debug font screen for 5s
	if (strcmp(cmd.c_str(), "font") == 0) {
		Glyphs::debugFont();
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