#include "states/MainMenu.h"

namespace MainMenu {
	MenuItem* buttons[8];

	void setup() {
		INFO("enter menu");
		buttons[0] = new MenuItem(PlayBtn, 7, 5);
		buttons[1] = new MenuItem(SpriteBtn, 110, 5);
		buttons[2] = new MenuItem(LogicBtn, 212, 5);
		buttons[3] = new MenuItem(RoomBtn, 7, 84);
		buttons[4] = new MenuItem(TextBtn, 110, 84);
		buttons[5] = new MenuItem(PaletteBtn, 212, 84);
		buttons[6] = new MenuItem(DataBtn, 7, 162);
		buttons[7] = new MenuItem(SettingsBtn, 110, 162);

		UIHelper::clearActive();
		for (MenuItem* btn : buttons) {
			UIHelper::registerUI(btn);
			btn->callback.bind(callback_menu);
		}
		touch->clearQueue();
		draw();
	}

	/// Once a main menu button has been pressed, the state changes
	void callback_menu(MenuItem& button) {
		if (button.shortcut == PlayBtn) {
			state = PlayState;
		} else if (button.shortcut == SpriteBtn) {
			state = SpriteEditorState;
		} else if (button.shortcut == LogicBtn) {
			state = LogicEditorState;
		} else if (button.shortcut == RoomBtn) {
			state = RoomEditorState;
		} else if (button.shortcut == TextBtn) {
			state = DialogEditorState;
		} else if (button.shortcut == PaletteBtn) {
			state = PaletteEditorState;
		} else if (button.shortcut == DataBtn) {
			state = DataManagementState;
		} else if (button.shortcut == SettingsBtn) {
			state = SettingsEditorState;
		}
		deallocate();
	}

	void loop() {
		UIHelper::loop();
	}

	void draw() {
		DrawingUtils::fill(TAN);
		drawRGBBitmap(215, 167, bottomRight, 91, 68, 0x0000);
		UIHelper::render();
	}

	void deallocate() {
		LEAK("menu dealloc start");
		UIHelper::clearActive();
		for (MenuItem* btn : buttons) {
			delete btn;
		}
		LEAK("menu dealloc end");
	}

} // namespace MainMenu
