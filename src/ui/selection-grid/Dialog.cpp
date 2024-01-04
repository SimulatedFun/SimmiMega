#include "ui/selection-grid/Dialog.h"

void DialogSelectionGrid::render() {
	Dialog dialog;
	for (uint8_t i = 0; i < dialogsPerTab; i++) {
        if (showZeroth) {
            dialog.dialogId = i + (*currentPage) * dialogsPerTab;
        } else {
            dialog.dialogId = i + (*currentPage) * dialogsPerTab + 1; // start from 1 instead
        }

        if (dialog.dialogId >= dialogCount) {
            display->fillRectangle(0, y + i * dialogPreviewHeight + 1, 311, dialogPreviewHeight - 1, RGB565(0x574b67));
            continue; // don't try and draw dialog previews past bounds
        }

        dialog.load();

		display->startWrite();
		{
			display->fillRectangleTx(0, y + i * dialogPreviewHeight, 311, dialogPreviewHeight,
											 RGB565(0x574b67));
			display->drawDottedRectangleTx(0, y + i * dialogPreviewHeight, 311, dialogPreviewHeight,
													 LIGHT_GREY);
		}
		display->endWrite();

		dialog.drawPreview(10, y + 16 + i * dialogPreviewHeight, 276, 1);
	}
}

/// Returns row that was selected
void DialogSelectionGrid::handlePress() {
	uint8_t row = getRelativeY() / dialogPreviewHeight;
	INFO(F("selected dialog row: ") << row);
	if (callback.function != nullptr) {
		callback.function(row);
	}
}