#ifndef Display_extra_H
#define Display_extra_H

#include <Arduino.h>
#include "spi/Display.h"
#include "GameObject.h"
#include "Dialog.h"
#include "Coordinates.h"
#include "Palette.h"

void drawGameObjectAtCoords(GameObject* g, Palette* p, Coordinates* c, u8 scale, boo frame, boo fullDraw);
void drawGameObject(GameObject* obj, Palette* p, u16 x, u16 y, u8 scale, boo frame, boo fullDraw);
void drawDialogBox(Dialog* d, u8 pageNumber, u8 topLeft_x = 12, u8 topLeft_y = 168, u8 scale = 2);

#endif