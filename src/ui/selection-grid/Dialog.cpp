#include "ui/selection-grid/Dialog.h"

const uint8_t previewHeight = 21;
const uint16_t bgColor = RGB565(0x574b67);

void DialogSelectionGrid::render() {
	Dialog dialog;
	for (uint8_t i = 0; i < dialogsPerTab; i++) {
        dialog.dialogId = i + (*currentPage) * dialogsPerTab;

        // if we're not zero-indexed, start from ID 1 instead (and offset all other IDs by one)
        if (!showZeroth) {
            dialog.dialogId++;
        }

        // draw a blank bg fill in place of the dotted rectangle if the obj is out of bounds
        if (dialog.dialogId >= dialogCount) {
            display->fillRectangle(0, y + i * previewHeight + 1, 312, previewHeight, bgColor);
            continue; // don't try and draw dialog previews past bounds
        }

        dialog.load();

		display->startWrite();
        display->fillRectangleTx(0, y + i * previewHeight, 311, previewHeight, bgColor);
        display->drawDottedRectangleTx(0, y + i * previewHeight, 311, previewHeight, LIGHT_GREY);
		display->endWrite();

		dialog.drawPreview(10, y + 16 + i * previewHeight, 276, 1);
	}
}

/// Returns row that was selected
void DialogSelectionGrid::handlePress() {
	uint8_t row = getRelativeY() / previewHeight;
	INFO(F("selected dialog row: ") << row);
	if (callback.function != nullptr) {
		callback.function(row);
	}
}