#include "ui/selection-grid/Dialog.h"

void DialogSelectionGrid::render() {
	Dialog dialog(0);
	for (uint8_t i = 0; i < dialogsPerTab; i++) {
		display->startWrite();
		{
			display->fillRectangleTx(0, y + i * dialogPreviewHeight, 311, dialogPreviewHeight,
											 RGB565(0x574b67));
			display->drawDottedRectangleTx(0, y + i * dialogPreviewHeight, 311, dialogPreviewHeight,
													 LIGHT_GREY);
		}
		display->endWrite();
		dialog.dialogId = i + (*currentPage) * dialogsPerTab;
		dialog.load();
		dialog.drawPreview(10, y + 16 + i * dialogPreviewHeight, 276, 1);
	}
}

void DialogSelectionGrid::handlePress() {
	const uint8_t row = getRelativeY() / dialogPreviewHeight;
	INFO(F("selected dialog row: ") << row);
	if (callback.function != nullptr) {
		callback.function(row);
	}
}