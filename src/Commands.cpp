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

	Serial.printf("read command: %s\n", cmd);

    // print out all available commands
	if (strncmp(cmd, "help", 4) == 0) {
		help();
		return;
	}

    // prints out the free ram
	if (strncmp(cmd, "ram", 3) == 0) {
		Serial.printf("free memory: %d", esp_get_minimum_free_heap_size());
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

    // list out all files in the main directory
	if (strncmp(cmd, "ls", 2) == 0) {
		microSd->begin();
		microSd->listDirTx("/", 10);
		microSd->end();
		return;
	}

    // show the contents of a file in the console
	if (strncmp(cmd, "cat", 3) == 0) {
		microSd->begin();
		microSd->readFile(&cmd[4]);
		microSd->end();
		return;
	}

    // remove a file from the SD card
    if (strncmp(cmd, "rm", 2) == 0) {
        microSd->begin();
        microSd->deleteFile(&cmd[3]);
        microSd->end();
        return;
    }
}

void Commands::help() {
	RAW("\n=== SimmiMega Help ===\nAvailable commands:\n");
	RAW("ram - prints available SRAM\n");
	RAW("ls - lists all files/directories on sd card\n");
	RAW("cat \"filename\" - prints contents of a file\n");
	RAW("rm \"filename\" - deletes a file\n");
	RAW("calibrate - calibrates the touch screen\n");
	RAW("font - draws every glyph on screen\n");
}