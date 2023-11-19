#ifndef states_h
#define states_h

enum State {
	OffState,				// go from this state to another to refresh
	PlayState,				// initializes engine and plays the game
	MainMenuState,			// shows main menu and navigation ui
	SpriteEditorState,	// opens the sprite editor to draw and change palette type
	LogicEditorState,		// opens the logic editor to change logic
	RoomEditorState,		// opens the room editor to place down objects, select music, etc
	DialogEditorState,	// edits dialog for use in logic editor
	DataManagementState, // manages data on the SD card
	PaletteEditorState,	// opens palette editor and creates palettes with a color wheel
	SettingsEditorState, // opens settings editor to change game settings
	SavedGamesState		// shows games stored on SD to load/duplicate/delete
};

extern State state;
extern State oldState;
extern boolean stateChange;

#endif