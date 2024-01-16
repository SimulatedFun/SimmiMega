#include "ui/selection-grid/Object.h"

void ObjectSelectionGrid::render() {
	/// Draws a tray of gameobjects (12 col x 7 row)
	Palette* pal = new Palette(WHITE, DARK_GREY, BLACK);
	for (u8 y = 0; y < 7; y++) {
		for (u8 x = 0; x < 12; x++) {
			const u16 xPos = 1 + (x * 26);
			const u16 yPos = 35 + 1 + (y * 26);
			u16 id = y * 12 + x;
			id += (*currentPage) * objectsPerTab;

            // if not showing the zeroth obj, offset all ids by 1
            if (!showZeroth) {
                id++;
            }

            // if obj OOB, fill with bg color and skip
            if (id >= objectCount) {
                display->fillRectangle(xPos, yPos, 24, 24, RGB565(0x574b67));
                continue;
            }

			GameObject* obj = new GameObject(id);
			obj->load();
			drawGameObject(obj, pal, xPos, yPos, 3, first_frame, true);
			delete obj;
		}
	}
	delete pal;
}

void ObjectSelectionGrid::handlePress() {
	const tsPoint_t pt = getRelativePoint();
	const u8 col = pt.x / 26; // 0 through 11
	const u8 row = pt.y / 26; // 0 through 6
	INFO("col: " << col << ", row: " << row);
	const u8 index = row * 12 + col;
	INFO(F("selected gameobject id: ") << index);

    // skip press event if id is OOB
    if (index >= objectCount) {
        return;
    }

	callback.function(index);
}