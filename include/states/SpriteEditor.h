#ifndef SpriteEditor_h
#define SpriteEditor_h

#include <Arduino.h>
#include "globals/Typedefs.h"

#include "GameObject.h"
#include "spi/Display.h"
#include "DrawBMP.h"
#include "globals/Async.h"
#include "states/States.h"

#include "ui/UIHelper.h"
#include "ui/RoundButton.h"
#include "ui/windows/ChooseObject.h"
#include "ui/ValueButton.h"
#include "ui/TrayElement.h"

constexpr boolean pencilTool = true;
constexpr boolean eraserTool = false;

namespace SpriteEditor {
	void setup();
	void loop();
	void draw();
	void setupUI();
	void deallocate();

	void drawSpritePixel(uint16_t index);
	void drawSpriteEditArea();

	void callbackEditSprite(uint8_t index);
	void callbackChangeFrame(Frame inFrame);
	void callbackExit(RoundButton&);
	void callbackEraser(RoundButton&);
	void callbackPencil(RoundButton&);
	void callbackInvert(RoundButton&);
	void callbackChangePalette(RoundButton&);
	void callbackCopy(RoundButton&);
	void callbackPaste(RoundButton&);
	void callbackClear(RoundButton&);

	uint16_t foregroundColor(GameObject* obj);

} // namespace SpriteEditor

#endif