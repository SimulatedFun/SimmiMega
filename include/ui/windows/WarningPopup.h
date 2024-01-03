#ifndef WarningBox_h
#define WarningBox_h

#include <Arduino.h>
#include "Glyphs.h"
#include "Audio.h"
#include "ui/SimpleButton.h"
#include "ui/UIHelper.h"
#include "ui/RoundButton.h"

enum WarningType : uint8_t { ExitPlayWarning, NewProjectWarning, SaveOverwriteWarning };
namespace WarningBox {
	void setup();
	void setupUI();
	void drawWindow();
	void deallocate();

	bool choose(Text*, Text*, Text*, BtnColor = {}, BtnColor = {});
	bool showMessage(Text* msg, Text* btnText, BtnColor btnColor = {});

	void callbackMakeChoice(boolean inChoice);
	void callbackExit(RoundButton&);

	void setupProgressBar(uint16_t steps, uint16_t barBg = RGB565(0xd4ffc8));
	void stepProgressBar(uint16_t barInner = RGB565(0x32e020));
	void finishProgressBar(uint16_t color = RGB565(0x32e020));
} // namespace WarningBox

#endif