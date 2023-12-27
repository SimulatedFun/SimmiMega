#include "states/SettingsMenu.h"
#include "Validate.h"

namespace SettingsMenu {
	RoundButton* exit;
	DialogSelector* dialog;
	RoomSelector* room;
	XYSelector* xy;
	TextFieldSelector *titleField, *descriptionField, *directoryField;
	WordTabElement *projectSettingsTab, *systemSettingsTab;

	void setup() {
		LEAK(F("data menu setup"));
		DrawingUtils::fill(RGB565(0x574b67));
		validateData();
		setupUI();
		loadData();
		draw();
		touch->clearQueue();
	}

	void setupUI() {
		exit = new RoundButton(ExitButton);
		exit->setPosition(288, 0);
		exit->callback.bind(&callbackExit);
		UIHelper::registerUI(exit);

		projectSettingsTab = new WordTabElement(ProjectSettingsTab);
		projectSettingsTab->setPosition(0, 219);
		projectSettingsTab->width = 156;
		projectSettingsTab->callback.bind(&callbackChangeTab);
		UIHelper::registerUI(projectSettingsTab);

		systemSettingsTab = new WordTabElement(SystemSettingsTab);
		systemSettingsTab->setPosition(156, 219);
		systemSettingsTab->width = 156;
		systemSettingsTab->callback.bind(&callbackChangeTab);
		UIHelper::registerUI(systemSettingsTab);

		dialog = new DialogSelector();
		dialog->setPosition(112, 176);
		dialog->width = 184;
		dialog->height = 24;
		dialog->callback.bind(&callbackDialog);
		UIHelper::registerUI(dialog);

		room = new RoomSelector();
		room->setPosition(115, 147);
		room->callback.bind(&callbackChooseRoom);
		UIHelper::registerUI(room);

		xy = new XYSelector();
		xy->setPosition(174, 147);
		xy->callback.bind(&callbackChooseXY);
		UIHelper::registerUI(xy);

		directoryField = new TextFieldSelector();
		directoryField->setPosition(112, 48);
		directoryField->callback.bind(&callbackEditDirectory);
		UIHelper::registerUI(directoryField);

		titleField = new TextFieldSelector();
		titleField->setPosition(112, 80);
		titleField->callback.bind(&callbackEditTitle);
		UIHelper::registerUI(titleField);

		descriptionField = new TextFieldSelector();
		descriptionField->setPosition(112, 112);
		descriptionField->callback.bind(&callbackEditDescription);
		UIHelper::registerUI(descriptionField);
	}

	void validateData() {
		uint8_t dialogId = _NO_DIALOG;
		GameSettings::getStartingDialog(&dialogId);
		if (!Validate::isValidDialogId(dialogId)) {
			Text* msg = new Text(F("Invalid initial dialog.\n\nDialog id is out of bounds."));
			Text* btn = new Text(F("Okay"));
			WarningBox::showMessage(msg, btn, greyBtn);
			delete msg;
			delete btn;
		}

		Coordinates coords;
		GameSettings::getStartingCoords(&coords);
		if (!Validate::isValidCoordinates(coords)) {
			Text* msg = new Text(F("Invalid starting position.\n\nEither x, y or room id is out of bounds."));
			Text* btn = new Text(F("Okay"));
			WarningBox::showMessage(msg, btn, greyBtn);
			delete msg;
			delete btn;
		}

		Folder folder;
		GameSettings::getDirectory(&folder);
		if (!Validate::isValidDirectoryName(folder.text)) {
			Text* msg = new Text(F("Invalid directory name:\n\n1. Directory name must be at least one character long\n2. Can contain letters, numbers or - _ or spaces\n3. Cannot start with a special character"));
			Text* btn = new Text(F("Okay"));
			WarningBox::showMessage(msg, btn, greyBtn);
			delete msg;
			delete btn;
		}
	}

	void loadData() {
		projectSettingsTab->selected = true;

		uint8_t dialogId = _NO_DIALOG;
		GameSettings::getStartingDialog(&dialogId);
		dialog->setData(dialogId);

		Coordinates coords;
		GameSettings::getStartingCoords(&coords);
		room->setData(coords.roomId);
		xy->setData(coords);

		Folder folder;
		GameSettings::getDirectory(&folder);
		const uint8_t dirLen = GameSettings::getStringLength(folder.text, directoryMaxLength);
		directoryField->text = new Text(folder.text, dirLen);

		Title title;
		GameSettings::getTitle(&title);
		const uint8_t titleLen = GameSettings::getStringLength(title.text.text, titleMaxLength);
		titleField->text = new Text(title.text.text, titleLen);

		Description desc;
		GameSettings::getDescription(&desc);
		const uint8_t descLen = GameSettings::getStringLength(desc.text, descriptionMaxLength);
		descriptionField->text = new Text(desc.text, descLen);
	}

	void deallocate() {
		delete exit;
		delete dialog;
		delete room;
		delete xy;

		delete titleField;
		delete descriptionField;
		delete directoryField;

		delete titleField->text;
		delete descriptionField->text;
		delete directoryField->text;

		delete projectSettingsTab;
		delete systemSettingsTab;
	}

	void loop() {
		UIHelper::loop();
	}

	void draw() {
		DrawingUtils::fill(RGB565(0x574b67));
		DrawingUtils::drawTitle(F("Project Settings"));

		display->startWrite();
		{
			// tan rectangular text area
			display->fillRectangleTx(8, 40, 296, 168, RGB565(0xeac592));
			display->drawPixelTx(8, 40, RGB565(0x574b67));	  // tl
			display->drawPixelTx(303, 40, RGB565(0x574b67));  // tr
			display->drawPixelTx(303, 207, RGB565(0x574b67)); // br
			display->drawPixelTx(8, 207, RGB565(0x574b67));	  // bl

			// bmp icon
			display->fillRectangleTx(17, 63, 32, 32, RGB565(0x574b67));
			display->drawHorizontalLineTx(17, 62, 32, RGB565(0x9d886e)); // top
			display->drawVerticalLineTx(16, 63, 32, RGB565(0x9d886e));	 // left
			display->drawHorizontalLineTx(17, 95, 32, WHITE);				 // bottom
			display->drawVerticalLineTx(49, 63, 32, WHITE);					 // right

			// start pos bg
			display->fillRectangleTx(112, 144, 184, 24, RGB565(0x2d1b2e));
			display->drawPixelTx(112, 144, RGB565(0xeac592)); // tl
			display->drawPixelTx(295, 144, RGB565(0xeac592)); // tr
			display->drawPixelTx(295, 167, RGB565(0xeac592)); // br
			display->drawPixelTx(112, 167, RGB565(0xeac592)); // bl
		}
		display->endWrite();

		// todo if no icon, else draw the icon
		DrawingUtils::simplePrint(26, 77, F("no"), RGB565(0xbababa), 1);
		DrawingUtils::simplePrint(23, 88, F("icon"), RGB565(0xbababa), 1);

		DrawingUtils::simplePrint(22, 57, F("Icon:"), RGB565(0x574b67), 1);
		DrawingUtils::simplePrint(73, 64, F("Folder:"), RGB565(0x574b67), 1);
		DrawingUtils::simplePrint(82, 96, F("Title:"), RGB565(0x574b67), 1);
		DrawingUtils::simplePrint(49, 128, F("Description:"), RGB565(0x574b67), 1);
		DrawingUtils::simplePrint(37, 160, F("Start Position:"), RGB565(0x574b67), 1);
		DrawingUtils::simplePrint(163, 161, F("@"), RGB565(0x574b67), 1);
		DrawingUtils::simplePrint(46, 192, F("Start Dialog:"), RGB565(0x574b67), 1);

		// separator above the tabs
		display->startWrite();
		{
			display->drawHorizontalLineTx(0, 218, 312, RGB565(0x937ac5));
			display->drawHorizontalLineTx(0, 217, 312, RGB565(0x2d1b2e));
		}
		display->endWrite();

		UIHelper::render();
	}

	void callbackExit(RoundButton&) {
		deallocate();
		state = MainMenuState;
	}

	void callbackDialog(boolean) {
		deallocate();

		const uint8_t dialogId = ChooseDialog::pick();
		if (dialogId != _NO_DIALOG) {
			GameSettings::setStartingDialog(dialogId);
		}

		// hacky avoid heap frag
		oldState = MainMenuState;
		state = SettingsEditorState;
	}

	void callbackChooseRoom(boolean) {
		deallocate();

		Coordinates coords;
		GameSettings::getStartingCoords(&coords);

		coords.roomId = ChooseRoom::pick();
		const uint8_t roomId = ChooseRoom::pick();
		if (roomId == _NO_ROOM) {
			oldState = MainMenuState;
			state = SettingsEditorState;
			return;
		}
		coords.roomId = roomId;
		GameSettings::setStartingCoords(coords);

		// hacky avoid heap frag
		oldState = MainMenuState;
		state = SettingsEditorState;
	}

	void callbackChooseXY(boolean) {
		deallocate();

		Coordinates coords;
		GameSettings::getStartingCoords(&coords);

		coords = ChooseXY::pick(coords.roomId);
		GameSettings::setStartingCoords(coords);

		// hacky avoid heap frag
		oldState = MainMenuState;
		state = SettingsEditorState;
	}

	void callbackEditDirectory(boolean) {
		INFO(F("callbackEditDirectory"));
		deallocate();

		Folder folder;
		GameSettings::getDirectory(&folder);
		const boolean saveChanges = EditTextField::editField(folder.text, directoryMaxLength, directoryMode);
		if (saveChanges) {
			INFO(F("saving changes to directory name"));
			GameSettings::setDirectory(folder);
			// todo verify we can change folder names like this
			//  move files, make backups, copy data, etc if necessary
		} else {
			INFO(F("discarding text changes"));
		}

		// hacky avoid heap frag
		oldState = MainMenuState;
		state = SettingsEditorState;
	}

	void callbackEditTitle(boolean) {
	}
	void callbackEditDescription(boolean) {
	}
	void callbackChangeTab(LogicTab tab) {
		if (tab == ProjectSettingsTab) {
			projectSettingsTab->selected = true;
			systemSettingsTab->selected = false;
		} else {
			// system settings
			projectSettingsTab->selected = false;
			systemSettingsTab->selected = true;
		}
		projectSettingsTab->render();
		systemSettingsTab->render();
	}


} // namespace SettingsMenu