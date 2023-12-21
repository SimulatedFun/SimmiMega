#ifndef PlayState_h
#define PlayState_h

#include <Arduino.h>
#include "GameObject.h"
#include "globals/DataBounds.h"
//#include "Audio.h"
#include "Settings.h"
#include "Dialog.h"
#include "Palette.h"
#include "states.h"
#include "RoomHelper.h"
#include "spi/Display.h"
#include "spi/MicroSD.h"
#include "globals/Async.h"
#include "DisplayExtra.h"
#include "ui/windows/WarningPopup.h"
#include "AStar.h"
#include "GameSettings.h"

using direction = uint8_t;
constexpr direction no_change = 0;
constexpr direction move_up = 1;
constexpr direction move_down = 2;
constexpr direction move_left = 3;
constexpr direction move_right = 4;

constexpr uint8_t roomWidth = 13;
constexpr uint8_t roomHeight = 10;

namespace Play {
	void setup();
	void loop();
	void allocate();
	void deallocate();

	void showBeginningDialog();
	void enterRoom(uint8_t roomId);
	void playRoomMusic(uint8_t roomId);
	void drawRoomTileAsync(uint8_t roomId, boolean fullDraw = false);
	void drawFlagUpdate(uint8_t flagId, uint8_t roomId);
	void drawAnimatedDialogArrow(uint16_t yPos);

	// Debug play mode
	void stateOfPlay();

	// Starting game or loading new room
	void resetGameVariables();
	void loadRoomVariables(uint8_t roomId);

	// Starts separate loop for specifically showing dialog boxes
	void showDialogSequence(uint8_t dialogId, uint8_t location = dialogBottom, boolean drawBehind = true);

	// Checks for button presses
	direction pollInput();

	// Sets newCoordinates which is where the player is trying to move to
	Coordinates getPlayerTargetLocation(uint8_t playerIntention);

	boolean isSolid(GameObject* g);
	boolean objectTriggersFlagChange(GameObject g);
	boolean objectInUpdatedState(GameObject* g);
	boolean isPlayerCoordinates(Coordinates* c);
	boolean displaceObject(GameObject* g);
	MovementType getMovementType(GameObject* g);
	boolean objectShowsDialog(GameObject g);
	uint8_t collideUpdateFlag(GameObject g);
	boolean objectTeleportsPlayer(GameObject g);
	boolean isAreaClear(Coordinates c);
	boolean getFrameToDraw(GameObject* g, boolean frame);
	boolean getFrameWithoutUpdate(GameObject* g, boolean frame);

	boolean playerWantsToMove();
	boolean playerWantsToChangeRooms();
	void movePlayerInSameRoom();

	void forceRoomFullDraw(uint8_t roomId);

	// Cached data from the play field, need to call an update every room change or flag update
	struct DynamicObject {
			uint8_t originalX : 4;
			uint8_t originalY : 4;
			uint8_t wanderOriginX : 4; // if we go from pathfinding to wandering far away from original x/y
			uint8_t wanderOriginY : 4;
			uint8_t x : 4;
			uint8_t y : 4;
			uint16_t gameObjectId : 10;
			MovementType type : 2;
			boolean solid;
			boolean cachedPathfindingFailure;
	};

	void nullifyDynamicObjects();
	void deallocateDynamicObjects();
	void addToDynamicObjects(DynamicObject* newObj);
	void allocateDynamicObjects();
	uint8_t dynamicGameObjectIdAtCoords(Coordinates c);
	boolean isDynamicObjectAtPosition(Coordinates c);
	boolean isDynamicObjectOriginButItMoved(Coordinates c);
	void asyncHandleWanderingObjects();
	void asyncHandlePathfindingObjects();

	GameObject getObjectAtCoord(Coordinates position, boolean considerDynamic = true, boolean considerPlayer = true);
	boolean isAnimated(GameObject* obj);
} // namespace play

extern boolean showPathways;
extern boolean profileTargetPathfinding;
extern boolean turnOnWalkableDisplay;

#endif
