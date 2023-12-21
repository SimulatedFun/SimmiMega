#ifndef WarningBox_h
#define WarningBox_h

#include <Arduino.h>
#include "Glyphs.h"
#include "Audio.h"
#include "ui/SimpleButton.h"
#include "ui/UIHelper.h"
#include "ui/RoundButton.h"

enum WarningType {ExitPlayWarning, NewProjectWarning, SaveOverwriteWarning};
namespace WarningBox {
	void setup();
	void setupUI();
	void drawWindow();
	void deallocate();

	bool choose(Text*, Text*, Text*, BtnColor = {}, BtnColor = {});
	bool showMessage(Text* msg, Text* btnText, BtnColor btnColor = {});

	void callbackMakeChoice(boolean inChoice);
	void callbackExit(RoundButton&);
}

#endif