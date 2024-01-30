#include "states/Play.h"
#include "FileManager.h"

// Terminology:
// "Gameobject" - a data structure that represents a single object sitting in a room or rooms. A
//    gameobject consists of two 8x8 sprites and as of now about 88 bits worth of logic that tell
//    the engine how to handle this object when rendering, on collision, how it moves, how it
//    responds to flag updates, etc.
// "Room" - a data structure consisting of a 13x10 grid of gameobjects, a song id for background
//    music, a game object id to know what object to use as the player sprite, and a palette id to
//    know what palette to use when rendering the objects.
// "Dialog" - a simple char array used by gameobjects to display dialog on collision, etc.
// "Palette" - simply three rgb colors used by the renderer to draw the objects in a room

// High-level Game Structure/Flow
// The game is made up of two "layers". On the first layer there are gameobject IDs at every
// position in a room and the second layer are dynamic objects which track moving objects. During
// play, the objects in the room are animated and the moving objects move one square at a time (
// either wandering about their initial location or targeting the player using the A* pathfinding
// algorithm).

// Todo for Movement Implementation:
// - handle player trying to walk into/through a moving object in its current spot (check if solid?)
// - handle taking up all the objects that need to move. Maybe have the nodes track what they are?
// - Add node game objectId and also index of the node in the room (index = where the obj
//    originated, id for collision and drawing functions)
// - Make drawing functions draw the moving objects first and not where they originated from
// - If object goes from moving to still, keep them in their current spot
// - Make a generalized pathfinding algorithm, taking into account other moving nodes and treat them
//    always as solid so we dont get nodes overlapping
// Before committing to a move, always check for solidity

namespace Play {
	Coordinates playerCoords; // where the player is currently standing
	Coordinates newCoords;	  // where the player is attempting to move to
	Coordinates drawCoords;	  // where the async draw calls are currently drawing

	// frame the draw calls are currently on varies from frame 1 to frame 2 (0 -> 1)
	// changes when the room is done being drawn on the previous frame
	frame currentFrame;

	// "flags" or variables/conditions that can be changed by the player when
	// interacting with objects. flags can trigger other objects to become displaced,
	// change frame, change solidity etc.
	boolean flag[flagCount]; // todo minimize the ram space of this by using bitfields/Bits::testBits

	// The object that represents the player sprite - can change per-room
	// only the sprite is used, logic is not used
	GameObject playerGameObj(0);

	// palettes only change when changing rooms, so store the current palette here
	Palette roomPalette;

	DynamicObject* dynamicObjects[130];
	uint8_t dynamicObjectLength = 0;

	DynamicObject* getDynamicObjectAtPosition(uint8_t x, uint8_t y) {
		for (uint8_t i = 0; i < dynamicObjectLength; i++) {
			if (dynamicObjects[i] == nullptr) {
				continue;
			}
			if (dynamicObjects[i]->x == x and dynamicObjects[i]->y == y) {
				return dynamicObjects[i];
			}
		}
		return nullptr;
	}

	void nullifyDynamicObjects() {
		for (uint8_t i = 0; i < 130; i++) {
			dynamicObjects[i] = nullptr;
		}
		dynamicObjectLength = 0;
	}

	/// Call this when leaving a room
	void deallocateDynamicObjects() {
		LEAK(F("deallocateDynamicObjects start"));
		for (uint8_t i = 0; i < 130; i++) {
			if (dynamicObjects[i] != nullptr) {
				delete dynamicObjects[i];
			}
		}
		dynamicObjectLength = 0;
		LEAK(F("deallocateDynamicObjects end"));
	}

	void addToDynamicObjects(DynamicObject* newObj) {
		INFO(F("addToDynamicObjects index ")
			  << dynamicObjectLength << ", obj id: " << newObj->gameObjectId << " at (" << newObj->x
			  << ", " << newObj->y << ")");
		dynamicObjects[dynamicObjectLength++] = newObj;
	}

	void allocateDynamicObjects() {
		LEAK(F("allocateDynamicObjects start"));
		// if obj ever has movement then add them to the dynamic object list (no duplicates)
		for (uint8_t i = 0; i < 130; i++) {
			const uint8_t x = i % 13;
			const uint8_t y = i / 13;
			const Coordinates coord(x, y, playerCoords.roomId);

			// get the gameobject at this position in the room
			GameObject obj(_NO_GAMEOBJECT);
			obj = getObjectAtCoord(coord, false, false); // only works before dyn objs are instantiated

			// check if in the game's current state whether they're moving or not
			const MovementType movementType = getMovementType(&obj);
			const boolean solid = isSolid(&obj);

			if (movementType == movementStill) {
				DEBUG(F("not a dyn obj"));
				continue;
			}

			DEBUG("dyn obj detected!");

			DynamicObject* newObject = new DynamicObject();

			// make the new object
			newObject->originalX = x;
			newObject->originalY = y;
			newObject->wanderOriginX = x;
			newObject->wanderOriginY = y;
			newObject->x = x;
			newObject->y = y;
			newObject->gameObjectId = obj.id;
			newObject->type = movementType;
			newObject->solid = solid;
			newObject->cachedPathfindingFailure = false;

			// object is dynamic but not in the list, and is targeting the player right now
			addToDynamicObjects(newObject);
		}
		LEAK(F("allocateDynamicObjects end"));
		INFO(F("dyn objs: ") << dynamicObjectLength);
	}

	void flagUpdateDynamicObjects() {
		LEAK(F("flagUpdateDynamicObjects start"));
		for (uint8_t i = 0; i < 130; i++) {
			const uint8_t x = i % 13;
			const uint8_t y = i / 13;
			const Coordinates coord(x, y, playerCoords.roomId);

			// get the gameobject at this position in the room
			GameObject obj(_NO_GAMEOBJECT);
			obj = getObjectAtCoord(coord, false, false); // not considering dynamic objects

			// check if in the game's current state whether they're moving or not
			const MovementType movementType = getMovementType(&obj);
			const boolean solid = isSolid(&obj);

			// if it already exists, then just update the existing dyn obj
			// this will update the movement type for existing objects
			boolean updatedObj = false;
			for (uint8_t j = 0; j < dynamicObjectLength; j++) {
				if (dynamicObjects[j]->originalX != x or dynamicObjects[j]->originalY != y) {
					continue;
				}
				INFO(F("exists in dyn obj, let's update"));

				// set the new wander origin to the current position
				// we don't update the originalX and originalY because those are used to identify
				// where the object came from
				dynamicObjects[j]->wanderOriginX = dynamicObjects[j]->x;
				dynamicObjects[j]->wanderOriginY = dynamicObjects[j]->y;
				dynamicObjects[j]->gameObjectId = obj.id;
				dynamicObjects[j]->type = movementType;
				dynamicObjects[j]->solid = solid;
				dynamicObjects[j]->cachedPathfindingFailure = false;
				updatedObj = true;
				break;
			}
			if (updatedObj) {
				continue;
			}

			// don't add new still objects to the dyn objs list
			if (movementType == movementStill) {
				continue;
			}

			// make the new object
			DynamicObject* newObject = new DynamicObject();
			newObject->x = x;
			newObject->y = y;
			newObject->originalX = x;
			newObject->originalY = y;
			newObject->wanderOriginX = x;
			newObject->wanderOriginY = y;
			newObject->gameObjectId = obj.id;
			newObject->type = movementType;
			newObject->solid = solid;
			newObject->cachedPathfindingFailure = false;

			// object is dynamic but not in the list
			addToDynamicObjects(newObject);
		}
		LEAK(F("flagUpdateDynamicObjects end"));
		INFO(F("dyn objs: ") << dynamicObjectLength);
	}

	uint8_t dynamicGameObjectIdAtCoords(Coordinates c) {
		for (uint8_t i = 0; i < dynamicObjectLength; i++) {
			if (dynamicObjects[i] == nullptr) {
				ERROR(F("shouldn't be null! dynamicObjectLength wrong then"));
				return _NO_GAMEOBJECT; // no gaps in list
			}
			if (dynamicObjects[i]->x != c.x or dynamicObjects[i]->y != c.y) {
				continue;
			}
			return dynamicObjects[i]->gameObjectId;
		}
		return _NO_GAMEOBJECT;
	}

	/// Checks if there is an object in the dynamic object layer at the specified location
	/// \return if there is a dynamic object at the specified location
	boolean isDynamicObjectAtPosition(Coordinates c) {
		// never consider objects outside the room as being dynamic
		if (c.roomId != playerCoords.roomId) {
			return false;
		}

		return dynamicGameObjectIdAtCoords(c) != _NO_GAMEOBJECT;
	}

	boolean isDynamicObjectOriginButItMoved(Coordinates c) {
		// there are no dynamic objects in rooms until we enter them, and movement is erased when you
		// exit a room
		if (c.roomId != playerCoords.roomId) {
			return false;
		}

		// find the dynamic object that starts at the coordinate we want to check
		DynamicObject* originObj = nullptr;
		for (uint8_t i = 0; i < dynamicObjectLength; i++) {
			if (dynamicObjects[i] == nullptr) {
				ERROR(F("dynamicObjectLength null 34"));
				continue;
			}
			if (dynamicObjects[i]->originalX != c.x or dynamicObjects[i]->originalY != c.y) {
				continue;
			}
			originObj = dynamicObjects[i];
			break;
		}

		if (originObj == nullptr) {
			return false;
		}

		if (originObj->x != c.x or originObj->y != c.y) {
			// this object has moved and is no longer at its starting point
			return true;
		}

		// what about a new object being in the position? -> handle it beforehand
		return false;
	}

	void updateSurroundingNodesCache(Node* node) {
		Node* neighbors[4];
		const uint8_t neighborCount = AStar::getNeighborNodes(node, neighbors);
		for (uint8_t i = 0; i < neighborCount; i++) {
			DynamicObject* obj = getDynamicObjectAtPosition(neighbors[i]->x, neighbors[i]->y);
			if (obj == nullptr) {
				continue;
			}
			obj->cachedPathfindingFailure = false;
		}
	}

	// Handle pathfinding and moving and drawing one dynamic object's wander mechanic
	void asyncHandleWanderingObjects() {
		// no dynamic objects to move
		if (dynamicObjectLength == 0) {
			DEBUG(F("nothing to move"));
			return;
		}

		static uint8_t objsWandered = 0;
		static uint8_t currentObjectIndex = 0;
		DynamicObject* object = dynamicObjects[currentObjectIndex];

		// increments and loops back around if needed
		currentObjectIndex = (currentObjectIndex + 1) % dynamicObjectLength;

		// if we loop back around, reset the timer
		if (currentObjectIndex == 0) {
			if (objsWandered > 0) {
				DEBUG(F("objs wandered: ") << objsWandered);
			}
			resetTimer(WanderingDelay);
			objsWandered = 0;
		}

		if (object == nullptr) {
			ERROR(F("asyncHandleWanderingObjects object is null (bad length?)"));
			currentObjectIndex = 0;
			return;
		}

		// not handling these
		if (object->type == movementStill or object->type == movementTarget) {
			return;
		}

		DEBUG("obj " << object->gameObjectId << ", at (" << object->x << ", " << object->y
						 << ") wants to wander");

		const uint8_t x = object->x;
		const uint8_t y = object->y;

		if (object->gameObjectId == _NO_GAMEOBJECT) {
			DEBUG(F("move dyn obj id = 0"));
			return;
		}

		if (object->cachedPathfindingFailure) {
			return; // don't bother until something changes
		}

		// 25% chance to stand still
		if ((boolean) (random() % 4)) {
			return;
		}

		// randomize next direction and then handle moving

		// check if any position is available

		Node* neighbors[4];
		const ScreenPosition objectPosition{object->x, object->y};
		Node* current = AStar::getNode(objectPosition);
		const uint8_t neighborCount = AStar::getNeighborNodes(current, neighbors);

		// Set the origin - secondary origin is used when the object wanders away from its original
		// position (target mode) and then switches back to wander mode
		const ScreenPosition wanderOrigin{object->wanderOriginX, object->wanderOriginY};
		Node* origin = AStar::getNode(wanderOrigin);

		// calculate neighbors that are open to move to and not too far from origin
		Node* openNeighbors[4];
		uint8_t openNeighborCount = 0;
		for (uint8_t i = 0; i < neighborCount; i++) {
			Node* neighbor = neighbors[i];
			if (!neighbor->walkable) {
				continue;
			}
			// skip nodes that are too far from origin
			if (AStar::calculateDistance(origin, neighbor) > 4) {
				continue;
			}
			// don't include the player location in possible wandering moves
			if (neighbor->x == playerCoords.x and neighbor->y == playerCoords.y) {
				continue;
			}
			openNeighbors[openNeighborCount++] = neighbor;
		}

		// no open neighbors (or at least none within wandering distance)
		if (openNeighborCount == 0) {
			// reminder: if a spot is too far from origin, then it can't be an open neighbor
			object->cachedPathfindingFailure = true;
			return;
		}

		const uint8_t randomDirection = random() % openNeighborCount;

		// this is the node we want to move to
		const uint8_t newX = openNeighbors[randomDirection]->x;
		const uint8_t newY = openNeighbors[randomDirection]->y;

		// important to update dyn obj coords since getObjectAtCoord includes dyn obj handling
		object->x = newX;
		object->y = newY;

		Coordinates oldPosition(x, y, playerCoords.roomId);
		Coordinates newPosition(newX, newY, playerCoords.roomId);

		GameObject oldObj = getObjectAtCoord(oldPosition);
		GameObject newObj = getObjectAtCoord(newPosition);

		const boolean oldFrame = getFrameToDraw(&oldObj, currentFrame);
		const boolean newFrame = getFrameToDraw(&newObj, currentFrame);

		drawGameObjectAtCoords(&oldObj, &roomPalette, &oldPosition, 3, oldFrame, true);
		drawGameObjectAtCoords(&newObj, &roomPalette, &newPosition, 3, newFrame, true);

		// updates only the two grid places in the node grid that may have changed
		Node* oldNode = AStar::getNode(ScreenPosition{oldPosition.x, oldPosition.y});
		oldNode->walkable = Play::isAreaClear(oldPosition);

		for (uint8_t i = 0; i < neighborCount; i++) {
			DynamicObject* obj = getDynamicObjectAtPosition(neighbors[i]->x, neighbors[i]->y);
			if (obj == nullptr) {
				continue;
			}
			obj->cachedPathfindingFailure = false;
		}

		Node* newNode = AStar::getNode(ScreenPosition{newPosition.x, newPosition.y});
		newNode->walkable = Play::isAreaClear(newPosition);

		objsWandered++;
	}

	// todo fix up these arguments and make it more modular - double check for bugs
	void handleCollisionWithObject(GameObject objectAtNewLocation, boolean* playerMovedInSameRoom) {
		// 2. Show dialog if needed
		// if the object shows dialog, open up the dialog box and pause everything else except for
		// animation until the box is closed
		if (objectShowsDialog(objectAtNewLocation)) {
			DEBUG("obj shows dialog");

			// todo get dialog alignment
			showDialogSequence(objectAtNewLocation.touchDialogId(), dialogBottom, true);

			// todo only overdraw the spots where the dialog box was instead
			forceRoomFullDraw(playerCoords.roomId);
		}

		// 3. Update flags
		if (objectTriggersFlagChange(objectAtNewLocation)) {
			DEBUG("obj triggers flag");
			const uint8_t flagId = collideUpdateFlag(objectAtNewLocation);

			displaceObject(&playerGameObj); // check just in case

			// fixme: overwrites position erroneously
			flagUpdateDynamicObjects(); // updates not just allocating

			// todo only redraw objects that need redrawing, but for now we're just going to draw the
			// entire room
			drawFlagUpdate(flagId, playerCoords.roomId);
			DEBUG(F("drawFlagUpdate done"));
		}

		// 4. Teleport to room
		if (objectTeleportsPlayer(objectAtNewLocation)) {
			DEBUG("objectTeleportsPlayer");
			const Coordinates tpCoords = objectAtNewLocation.getTeleportLocation();
			DEBUG("got coords");

			DEBUG("obj tps player to (" << tpCoords.x << ", " << tpCoords.y
												 << ") roomId: " << tpCoords.roomId);
			if (isAreaClear(tpCoords)) {
				DEBUG("tp location clear");

				// set new coordinates
				newCoords = tpCoords;
				playerCoords = tpCoords;
				*playerMovedInSameRoom = false;

				// load room and redraw
				DEBUG(F("play 527"));
				enterRoom(playerCoords.roomId, false);
			}
		}
	}

	// Handle pathfinding and moving and drawing one dynamic object's player targeting mechanic
	void asyncHandlePathfindingObjects() {
		// no dynamic objects to move
		if (dynamicObjectLength == 0) {
			DEBUG(F("nothing to move"));
			return;
		}

		static uint8_t objectsMoved = 0;

		static uint8_t currentObjectIndex = 0;
		DynamicObject* object = dynamicObjects[currentObjectIndex];

		// increments and loops back around if needed
		currentObjectIndex = (currentObjectIndex + 1) % dynamicObjectLength;

		// if we loop back around, reset the timer
		if (currentObjectIndex == 0) {
			if (objectsMoved > 0) {
				DEBUG(F("async targeted: ") << objectsMoved);
			}
			objectsMoved = 0;
			resetTimer(PathfindingDelay);
		}

		if (object == nullptr) {
			ERROR(F("asyncHandlePathfindingObjects object is null (bad length?)"));
			currentObjectIndex = 0;
			return;
		}

		// not handling these here
		if (object->type == movementStill or object->type == movementWander) {
			return;
		}

		DEBUG(F("obj ") << object->gameObjectId << ", at (" << object->x << ", " << object->y
							 << ") wants to move");

		const uint8_t x = object->x;
		const uint8_t y = object->y;

		if (object->gameObjectId == _NO_GAMEOBJECT) {
			DEBUG(F("move dyn obj id = 0"));
			return;
		}

		if (object->cachedPathfindingFailure) {
			return; // don't bother until something changes
		}

		// calculate next direction and then handle moving

		static unsigned long start = 0;
		if (profileTargetPathfinding) {
			start = micros();
		}

		ScreenPosition newPos{0, 0};
		const ScreenPosition currentPos{x, y};
		const ScreenPosition playerPos{playerCoords.x, playerCoords.y};
		const bool success = AStar::nextMove(&newPos, currentPos, playerPos);
		if (!success) {
			DEBUG("(" << object->x << ", " << object->y << ")"
						 << F("either blocked or in best position"));
			return;
		}

		if (newPos.x == playerCoords.x and newPos.y == playerCoords.y) {

			//          INFO(F("collision triggered by targeting object"));


			const Coordinates pos(object->x, object->y, playerCoords.roomId);
			const GameObject obj = getObjectAtCoord(pos, true, false);
			boolean unused = false;
			handleCollisionWithObject(obj, &unused);
			return;
		}

		// important to update dyn obj coords since getObjectAtCoord includes dyn obj handling
		object->x = newPos.x;
		object->y = newPos.y;

		Coordinates oldPosition(x, y, playerCoords.roomId);
		Coordinates newPosition(newPos.x, newPos.y, playerCoords.roomId);

		GameObject oldObj = getObjectAtCoord(oldPosition);
		GameObject newObj = getObjectAtCoord(newPosition);

		const boolean oldFrame = getFrameToDraw(&oldObj, currentFrame);
		const boolean newFrame = getFrameToDraw(&newObj, currentFrame);

		drawGameObjectAtCoords(&oldObj, &roomPalette, &oldPosition, 3, oldFrame, true);
		drawGameObjectAtCoords(&newObj, &roomPalette, &newPosition, 3, newFrame, true);

		// updates only the two grid places in the node grid that may have changed
		Node* oldNode = AStar::getNode(ScreenPosition{oldPosition.x, oldPosition.y});
		oldNode->walkable = Play::isAreaClear(oldPosition);

		updateSurroundingNodesCache(oldNode); // un-caches failure

		Node* newNode = AStar::getNode(ScreenPosition{newPosition.x, newPosition.y});
		newNode->walkable = Play::isAreaClear(newPosition);

		objectsMoved++;
	}

	void setup() {
		DEBUG(F("setup"));
		nullifyDynamicObjects();
		DEBUG(F("nullifyDynamicObjects"));
		allocate();
		DEBUG(F("allocate"));
		resetGameVariables();
		DEBUG(F("resetGameVariables"));
		showBeginningDialog();
		DEBUG(F("todo REENABLE showBeginningDialog"));
		enterRoom(playerCoords.roomId, true);
		DEBUG(F("enterRoom"));

		resetTimer(ButtonMoveDelay);			// wait 150ms before receiving input
		resetTimer(AsyncPlayModeAnimation); // screen redraws ever 500ms
		resetTimer(DialogArrowAnimation);	// animated dialog arrow
		resetTimer(PathfindingDelay);			// pathfinding targeting
		resetTimer(WanderingDelay);			// pathfinding wander
		touch->clearQueue();
		LEAK(F("setup done"));
	}

	/// Handle pressing on the screen during play mode, exits to the main menu
	/// Returns true if we want to return to the menu
	boolean exitingPlayMode() {
		Text* confirm = new Text(F("Exit"));
		Text* cancel = new Text(F("Cancel"));
		Text* main = new Text(F("Are you sure you want to exit?\n\nProgress will not be saved."));
		const boolean confirmExit = WarningBox::choose(confirm, cancel, main, redBtn, greyBtn);
		delete confirm;
		delete cancel;
		delete main;
		return confirmExit;
	}

	void showBeginningDialog() {
		INFO("showBeginningDialog in");
		// draw background color
		Palette pal;
		RoomHelper::getPaletteId(&pal.id, playerCoords.roomId);
		pal.load();
		DrawingUtils::fill(pal.getBackground());
		INFO(F("fill bg"));

		// there will always be a beginning dialog
		uint8_t dialogId = 0;
		GameSettings::getStartingDialog(&dialogId);
		showDialogSequence(dialogId, dialogMiddle, false);
	}

	void playRoomMusic(uint8_t roomId, boolean initialStart) {
        static uint8_t currentMusicId = 0;
		uint8_t musicId = 0;
		RoomHelper::getMusicId(&musicId, roomId);
		INFO(F("music id: ") << musicId);

        // don't restart track when moving between rooms with the same bgm
        if (!initialStart and musicId == currentMusicId) {
            return;
        }

		String filename;

		microSd->begin();
		if (FileManager::getSongInfoTx(musicId, &filename)) {
			ERROR(F("unable to get song info"));
		} else {
			INFO(F("playing song: ") << filename);
		}
		microSd->end();

		INFO(F("playing song: ") << filename);

		Folder gameFolder;
		GameSettings::getDirectory(&gameFolder);

		INFO(F("file path: ") << filename);
		Sound::playFile(String(gameFolder.text) + "/music/" + filename);
	}

	/// Draws a whole room and begins playing said room's music
	void enterRoom(uint8_t roomId, boolean initialStart) {
		DEBUG(F("enterRoom: ") << roomId);
		loadRoomVariables(roomId);
		stateOfPlay();
		forceRoomFullDraw(roomId);
		playRoomMusic(roomId, initialStart);
	}

	void allocate() {
		AStar::allocate();
	}

	void deallocate() {
		deallocateDynamicObjects();
		AStar::deallocate();
	}

	void resetGameVariables() {
		drawCoords.x = drawCoords.y = 0;

		Coordinates startingCoords;
		GameSettings::getStartingCoords(&startingCoords);
		newCoords = startingCoords;
		playerCoords = startingCoords;

		// Reset flags all to false
		for (uint8_t i = 0; i < flagCount; i++) {
			flag[i] = false;
		}
		currentFrame = first_frame;
	}

	void loadRoomVariables(uint8_t roomId) {
		DEBUG(F("loadRoomVariables for room id: ") << roomId);
		// set palette of room
		RoomHelper::getPaletteId(&roomPalette.id, roomId);
		roomPalette.load();

		// load per-room player gameobject
		RoomHelper::getPlayerGameObjectId(&playerGameObj.id, roomId);
		playerGameObj.load();
		displaceObject(&playerGameObj);

		// initialize all the walking objects on their own layer
		deallocateDynamicObjects();
		nullifyDynamicObjects();
		allocateDynamicObjects();
		AStar::resetGrid();
		AStar::updateObstacles(roomId);
	}

	// todo remake with respect to dyn objs
	void drawFlagUpdate(uint8_t flagId, uint8_t roomId) {
		DEBUG(F("drawFlagUpdate, flagId: ") << flagId << F(", roomId: ") << roomId);
		stateOfPlay();
		drawCoords.x = 0;
		drawCoords.y = 0;

		// todo selective redraw
		forceRoomFullDraw(playerCoords.roomId);

		// check what objects are affected by the flag update and only redraw those
		// todo an object might not have any effect, it might just be set to the flag as default
		//  ie an object might be set to be updated by the flag but might not have any visual changes
		//		for (uint8_t i = 0; i < objectsPerRoom; i++) {
		//			Sound::buffer();
		//
		//			// for each position in the room
		//			const uint16_t x = i % 13;
		//			const uint16_t y = i / 13;
		//			Coordinates position(x, y, roomId);
		//
		//			boolean redraw = false;
		//
		//			// get the object at the position
		//			GameObject g(0);
		//			if (isPlayerCoordinates(&position)) {
		//				g.id = playerGameObj.id;
		//				redraw = true;
		//				// todo dynamic moved away
		//			} else if (isDynamicObjectAtPosition(position)) {
		//				g.id = dynamicGameObjectIdAtCoords(position);
		//			} else {
		//				RoomHelper::getGameObjectId(&g.id, position);
		//			}
		//			g.load();
		//
		//			// if object is displaced, redraw
		//			if (displaceObject(&g)) {
		//				redraw = true;
		//			}
		//
		//			// if object isn't displaced but is updated by the flag
		//			// or if the displaced object is updated by the flag
		//			if (g.updateOnFlagId() == flagId) {
		//				if (g.updateAffectsAnimated() or g.updateAffectsFrame()) {
		//					redraw = true;
		//				}
		//			}
		//
		//			if (redraw) {
		//				// g.toString();
		//				const boolean frame = getFrameToDraw(&g, currentFrame);
		//				drawGameObjectAtCoords(&g, &roomPalette, &position, 3, frame, true);
		//			}
		//		}
		AStar::updateObstacles(playerCoords.roomId);
	}


	struct pinCorrelation {
			uint8_t pin;
			uint8_t direction;
	};
	pinCorrelation inputPins[4] = {{PIN_LEFT_BUTTON, move_left},
											 {PIN_DOWN_BUTTON, move_down},
											 {PIN_UP_BUTTON, move_up},
											 {PIN_RIGHT_BUTTON, move_right}};
	/// Checks button inputs and returns a direction the player wants to move
	direction pollInput() {
		if (!checkTimer(140, ButtonMoveDelay, false)) {
			return no_change;
		}
		for (uint8_t i = 0; i < 4; i++) {
			if (digitalRead(inputPins[i].pin)) {
				resetTimer(ButtonMoveDelay);
				return inputPins[i].direction;
			}
		}
		return no_change;
	}

	/// Sets newCoordinates to the location the player is attempting to move to
	/// \param playerIntention button press direction
	Coordinates getPlayerTargetLocation(uint8_t playerIntention) {
		Coordinates targetLocation = playerCoords;
		switch (playerIntention) {
			case move_left:
				// if player is anywhere in the room except the far left
				if (playerCoords.x > 0) {
					targetLocation.x = playerCoords.x - 1; // move them to the left
					return targetLocation;
				}

				// if player is trying to walk past the left edge of the left-most room, do nothing
				if (playerCoords.roomId % 6 == 0) {
					return targetLocation;
				}

				// if they're in the left-most column of the room and there's a room to the left, put
				// them in it
				targetLocation.x = 12;
				targetLocation.roomId = playerCoords.roomId - 1;
				return targetLocation;
			case move_right:
				// if player is anywhere in the room except the far right
				if (playerCoords.x < 12) {
					targetLocation.x = playerCoords.x + 1; // move them to the right
					return targetLocation;
				}

				// if player is trying to walk past the right edge of the right-most room, do nothing
				if ((playerCoords.roomId + 1) % 6 == 0) {
					return targetLocation;
				}

				// if they're in the right-most column of the room and there's a room to the right, put
				// them in it
				targetLocation.x = 0;
				targetLocation.roomId = playerCoords.roomId + 1;
				return targetLocation;
			case move_down:
				// if player is anywhere in the room except the bottom row
				if (playerCoords.y < 9) {
					targetLocation.y = playerCoords.y + 1; // move them down one
					return targetLocation;
				}

				// if player is trying to walk past the bottom edge of the bottom-most room, do nothing
				if (playerCoords.roomId >= 30) {
					return targetLocation;
				}

				// if in the bottom-most row of the room and there's a room below, put them in it
				targetLocation.y = 0;
				targetLocation.roomId = playerCoords.roomId + 6;
				return targetLocation;
			case move_up:
				// if player is anywhere in the room except the top row
				if (playerCoords.y > 0) {
					targetLocation.y = playerCoords.y - 1; // move them up one
					return targetLocation;
				}

				// if player is trying to walk past the top edge of the top-most room, do nothing
				if (playerCoords.roomId < 6) {
					return targetLocation;
				}

				// if in the top-most row of the room and there's a room above, put them in it
				targetLocation.y = 9;
				targetLocation.roomId = playerCoords.roomId - 6;
				return targetLocation;
			default:
				return targetLocation;
		}
	}

	/// Displaces a GameObject if its flag requirement is set
	/// \param g GameObject to displace
	boolean displaceObject(GameObject* g) {
		if (!objectInUpdatedState(g)) {
			return false;
		}

		if (!g->updateReplacesWithGameObject()) {
			return false;
		}

		g->id = g->updateReplacesWithGameObjectId();
		DEBUG("obj was displaced with gid: " << g->id);
		g->load(true);
		return true;
	}

	/// Gets the game object's movement type with respect to flag updates
	MovementType getMovementType(GameObject* g) {
		const MovementType initialType = g->initialMovementType();

		// if obj not even updated, it will have the initial movement setting
		if (!objectInUpdatedState(g)) {
			return initialType;
		}

		// obj is updated

		// if update doesn't affect movement, then return the initial
		if (!g->updateAffectsMovement()) {
			return initialType;
		}

		return g->updateSetsMovementTo();
	}

	/// Returns whether the object will show a dialog box based on game state
	/// \param g GameObject
	/// \return whether the object will show a dialog box based on game state
	boolean objectShowsDialog(GameObject g) {
		// if the object does not show dialog
		if (!g.touchShowsDialog()) {
			return false;
		}

		// if it does show dialog and doesn't require a flag
		if (!g.dialogRequiresFlag()) {
			return true;
		}

		// check if required flag is set
		const uint8_t flagId = g.dialogRequiresFlagId();
		const boolean flagValue = g.dialogRequiresFlagSetTo();
		if (flag[flagId] == flagValue) {
			return true;
		}

		// flag not set, doesn't show dialog
		return false;
	}

	boolean objectTriggersFlagChange(GameObject g) {
		// object doesn't trigger a flag update
		if (!g.touchTriggersFlag()) {
			DEBUG("touch doesn't trigger flag");
			return false;
		}
		DEBUG("touch triggers flag");

		// flipping flag always triggers an update
		if (g.touchFlipsFlag()) {
			DEBUG("touch flips flag");
			return true;
		}

		// if flag isn't flipped but set, if the value doesn't change, don't trigger an update
		if (flag[g.touchTriggersFlagId()] == g.touchSetsFlagTo()) {
			DEBUG("touch sets flag to: " << g.touchSetsFlagTo());
			return false;
		}

		// the value of the flag doesn't match the new value we're setting it to,
		// therefore trigger a flag update
		DEBUG("touch updates flag for sure");
		return true;
	}

	uint8_t collideUpdateFlag(GameObject g) {
		const uint8_t flagId = g.touchTriggersFlagId();
		if (g.touchFlipsFlag()) {
			flag[flagId] = !flag[flagId];
		} else {
			flag[flagId] = g.touchSetsFlagTo();
		}
		return flagId;
	}

	/// Checks if a GameObject will teleport a player once the player collides with it
	/// (accounting for flag updates)
	/// \param g GameObject
	/// \return whether the GameObject will teleport the player
	boolean objectTeleportsPlayer(GameObject g) {
		// if object doesn't teleport player
		if (!g.touchTeleportsToRoom()) {
			DEBUG(F("no teleport"));
			return false;
		}

		// if the object teleports the player and doesn't require a flag
		if (!g.touchTeleportRequiresFlag()) {
			return true;
		}

		// if object teleports but doesn't have required flag set
		if (flag[g.touchTeleportRequiresFlagId()] != g.touchTeleportRequiresFlagSetTo()) {
			return false;
		}

		// the object teleports the player and has the required flag set
		return true;
	}

	/// is the area clear to move the player into? tp or direction move
	/// considers dynamic moving objects too
	boolean isAreaClear(Coordinates c) {
		GameObject obj(_NO_GAMEOBJECT);
		obj = getObjectAtCoord(c);

		// if the object is solid, the area is not clear
		if (isSolid(&obj)) {
			DEBUG(F("solid object in the way (not clear, no tp)"));
			return false;
		}

		// area is clear
		return true;
	}

	boolean playerWantsToChangeRooms() {
		const boolean changingRooms = newCoords.roomId != playerCoords.roomId;
		if (changingRooms) {
			DEBUG(F("player wants to change rooms"));
		}
		return changingRooms;
	}

	// todo moving off the screen and teleporting share a lot of logic, maybe combine?
	// Player has moved +/- xy, and may have navigated out of the room or collided with an object
	// (possibly even off-screen) Purposeful decisions:
	// - redraw room if the player moves out of the room BEFORE drawing dialog, handling tp, flag
	// updates, etc.
	void movePlayer() {
		DEBUG(F("player has moved"));
		boolean playerMovedInSameRoom = true;

		// Load the object at the position where the player wants to move
		// could be in a different room
		GameObject objectAtNewLocation(_NO_GAMEOBJECT);
		objectAtNewLocation = getObjectAtCoord(newCoords);

		// region Debugging Messages
		if (playerWantsToChangeRooms()) {
			DEBUG(F("player wants to change rooms"));
		}
		if (isAreaClear(newCoords)) {
			DEBUG(F("area at new coords is clear"));
		} else {
			DEBUG(F("area at new coords is NOT clear"));
		}
		// endregion

		// 1. Check if player moved rooms and redraw that before anything else happens
		// if the player changed rooms and the new location isn't solid, we can redraw room first,
		// then handle the regular logic (showing dialog, tp, flag update, etc)
		// purposeful decision: changing rooms takes priority over teleporting the player
		if (playerWantsToChangeRooms() and isAreaClear(newCoords)) {
			DEBUG(F("player is moving off-screen"));
			playerMovedInSameRoom = false;

			// set new coordinates
			playerCoords = newCoords;

			// load room vars and redraw
			enterRoom(playerCoords.roomId, false);
		}

		handleCollisionWithObject(objectAtNewLocation, &playerMovedInSameRoom);

		// 5. Draw where player last was standing
		if (playerMovedInSameRoom and !isSolid(&objectAtNewLocation)) {
			DEBUG("player moved in same room");
			movePlayerInSameRoom();
		}
		DEBUG("movePlayer finished");
	}

	/// Changes player coords to new coords, redraws old spot and player in new spot, updated A*
	/// nodes
	void movePlayerInSameRoom() {
		Coordinates lastPosition = playerCoords;

		// update the player's coordinates
		playerCoords.x = newCoords.x;
		playerCoords.y = newCoords.y;

		// Get object under the player
		GameObject underGameObj(0);
		underGameObj = getObjectAtCoord(lastPosition);

		// draw where the player was standing prior
		const boolean otherFrame = getFrameToDraw(&underGameObj, currentFrame);
		drawGameObjectAtCoords(&underGameObj, &roomPalette, &lastPosition, 3, otherFrame, true);

		// draw the player in the new position
		const boolean playerFrame = getFrameToDraw(&playerGameObj, currentFrame);
		drawGameObjectAtCoords(&playerGameObj, &roomPalette, &playerCoords, 3, playerFrame, true);

		// make old spot walkable
		Node* oldSpot = AStar::getNode(ScreenPosition{lastPosition.x, lastPosition.y});
		oldSpot->walkable = isAreaClear(lastPosition); // rare but player could be moving off a solid

		updateSurroundingNodesCache(oldSpot);

		// make new player position not walkable
		Node* newSpot = AStar::getNode(ScreenPosition{playerCoords.x, playerCoords.y});
		newSpot->walkable = false;
	}

	void drawCachedFailure() {
		Serial << "drawCachedFailure:" << endl;
		for (uint8_t y = 0; y < 10; y++) {
			for (uint8_t x = 0; x < 13; x++) {
				DynamicObject* obj = getDynamicObjectAtPosition(x, y);
				if (obj == nullptr) {
					Serial << ". ";
					display->fillRectangle(x * 24 + 14, y * 24 + 14, 4, 4, GREEN);
					continue;
				}
				if (obj->cachedPathfindingFailure) {
					Serial << "X ";
					display->fillRectangle(x * 24 + 14, y * 24 + 14, 4, 4, RED);
				} else {
					Serial << ". ";
					display->fillRectangle(x * 24 + 14, y * 24 + 14, 4, 4, GREEN);
				}
			}
			Serial << endl;
		}
		Serial << endl;
	}

	void checkForOverlappingDynamicObjects() {
		// check for duplicate dynamic objects
		bool visited[13][10];
		memset(visited, 0, sizeof(visited));

		for (uint8_t i = 0; i < 130; i++) {
			if (dynamicObjects[i] != nullptr) {
				const uint8_t x = dynamicObjects[i]->x;
				const uint8_t y = dynamicObjects[i]->y;

				if (x < 13 && y < 10) {
					if (visited[x][y]) {
						DEBUG(F("Duplicate dynamic object found at (") << x << ", " << y << ")");
					} else {
						visited[x][y] = true;
					}
				}
			}
		}
	}

	void loopCheckForTouch() {
		static uint8_t loopCounter = 0;
		if (loopCounter++ == 15) {
			// touch->poll();
			if (touch->isPressed()) {
				DEBUG(F("menu from loop"));
				if (exitingPlayMode()) {
					Sound::stop();
					state = MainMenuState;
					deallocate();
					return;
				}
				// if we don't return to the menu, redraw the room
				forceRoomFullDraw(playerCoords.roomId);
			}
			loopCounter = 0;
		}
	}

	void loopPathfindingObjects() {
		if (checkTimer(250, PathfindingDelay, false)) {
			asyncHandlePathfindingObjects();
		}
	}

	void loopWanderingObjects() {
		if (checkTimer(1200, WanderingDelay, false)) {
			asyncHandleWanderingObjects();
		}
	}

	void loopAnimateTiles() {
		static unsigned long drawStart = 0;
		static unsigned long waitTime = 0;

		// if we're at the beginning, wait until 500ms have passed
		if (drawCoords.x == 0 and drawCoords.y == 0) {
			// restart animation timings start time
			drawStart = millis();

			if (!checkTimer(500, AsyncPlayModeAnimation, true)) {
				return; // not ready for animation
			}
		}

		drawRoomTileAsync(playerCoords.roomId, false);

		// when we finish a frame:
		if (drawCoords.x == 0 and drawCoords.y == 0) {
			if (500 < (millis() - drawStart)) {
				waitTime = 0;
			} else {
				waitTime = 500 - (millis() - drawStart);
			}
		}
	}

#ifdef ENABLE_TIMINGS
	unsigned long timings[8];
	constexpr uint8_t timingStart = 0;
	constexpr uint8_t timingInputPoll = 1;
	constexpr uint8_t timingMove = 2;
	constexpr uint8_t timingAudio = 3;
	constexpr uint8_t timingTarget = 4;
	constexpr uint8_t timingWander = 5;
	constexpr uint8_t timingAnimate = 6;
	constexpr uint8_t timingTouch = 7;
#endif

	void loopTimingsReport() {
#ifdef ENABLE_TIMINGS
		const unsigned long start = timings[timingStart];
		Serial << "\n== Timings Report ==" << endl;
		Serial << "Input poll  : " << timings[timingInputPoll] - start << endl;
		Serial << "Move player : " << timings[timingMove] - start << endl;
		Serial << "Audio buffer: " << timings[timingAudio] - start << endl;
		Serial << "Target AI   : " << timings[timingTarget] - start << endl;
		Serial << "Wander AI   : " << timings[timingWander] - start << endl;
		Serial << "Tile animate: " << timings[timingAnimate] - start << endl;
		Serial << "Touch poll  : " << timings[timingTouch] - start << endl;
		Serial << "====================" << endl;
#endif
	}

	void loop() {
		Sound::buffer();
#ifdef ENABLE_TIMINGS
		timings[timingStart] = micros();
		for (uint8_t i = 1; i < 8; i++) {
			timings[i] = 0;
		}
#endif

		// Check if the player wants to move (max every 140 millis)
		const uint8_t playerIntention = pollInput();
#ifdef ENABLE_TIMINGS
		timings[timingInputPoll] = micros();
#endif

		Sound::buffer();

		if (playerIntention != no_change) {
			// Set newCoordinates based on playerIntention
			newCoords = getPlayerTargetLocation(playerIntention);
			if (newCoords.fullEqual(playerCoords)) {
				// if player can't move due to screen bounds, cancel move player handling
				return;
			}
			movePlayer();
		}

		// If there's no input, just redraw a tile and restart the loop
		Sound::buffer();
#ifdef ENABLE_TIMINGS
		timings[timingAudio] = micros();
#endif

		loopPathfindingObjects();
#ifdef ENABLE_TIMINGS
		timings[timingTarget] = micros();
#endif

		Sound::buffer();

		loopWanderingObjects();
		// timings[timingWander] = micros();

		Sound::buffer();

		loopAnimateTiles();
		// timings[timingAnimate] = micros();

		Sound::buffer();

		// Check if we need to exit to the main menu
		// loopCheckForTouch();

		Sound::buffer();

		loopTimingsReport();
	}

	boolean isAnimated(GameObject* obj) {
		// check if the object is animated

		if (!obj->updateAffectsAnimated()) {
			return obj->isAnimated();
		}

		if (!objectInUpdatedState(obj)) {
			return obj->isAnimated();
		}

		return obj->updateSetsAnimatedTo();
	}

	frame getFrameToDrawUpdatedState(GameObject* g, frame inFrame) {
		const boolean affectsStartingFrame = g->updateAffectsFrame();
		const boolean affectsAnimated = g->updateAffectsAnimated();

		// get the final animation status
		boolean isAnimated = g->isAnimated();
		if (affectsAnimated) {
			isAnimated = g->updateSetsAnimatedTo();
		}

		// get the final starting frame
		frame startFrame = first_frame;
		if (g->startsOnFrameTwo()) {
			startFrame = second_frame;
		}
		if (affectsStartingFrame) {
			startFrame = g->updateSetsFrameTo();
		}

		// we should know if this object should be currently animated
		// and what frame [if/if not] to either start on or draw

		if (!isAnimated) {
			return startFrame;
		}

		// else is animated

		if (startFrame == first_frame) {
			return inFrame;
		} else {
			return !inFrame;
		}
	}

	frame getFrameToDrawNotUpdatedState(GameObject* g, frame inFrame) {
		if (g->isAnimated()) {
			if (g->startsOnFrameTwo()) {
				return !inFrame;
			}
			return inFrame;
		}

		// not animated
		if (g->startsOnFrameTwo()) {
			return second_frame;
		}
		return first_frame;
	}

	/// Finds what frame a game object should be drawn as with respect to flags & logic
	/// \param g GameObject to check
	/// \param frame current frame
	/// \return What frame we should draw for the GameObject g
	boolean getFrameToDraw(GameObject* g, boolean frame) {
		if (flag[g->updateOnFlagId()] == g->updateOnFlagSetTo()) {
			return getFrameToDrawUpdatedState(g, frame);
		}
		return getFrameToDrawNotUpdatedState(g, frame);
	}

	/// Checks if a GameObject is solid with respect to flags & logic
	/// \param g GameObject to check
	/// \return Whether the object is solid or not
	boolean isSolid(GameObject* g) {
		// if nothing modifies the solidity, return its starting value
		if (!g->updateAffectsSolidity()) {
			const boolean solid = g->isSolid();
			return solid;
		}

		// if an update does modify the solidity, but the obj hasn't been updated, return starting
		// solidity
		if (!objectInUpdatedState(g)) {
			const boolean solid = g->isSolid();
			return solid;
		}

		// if the solidity is updated and set to a value, return that value
		const boolean solid = g->updateSetsSolidityTo();
		return solid;
	}

	void drawAnimatedDialogArrow(uint16_t yPos) {
		if (!checkTimer(500, DialogArrowAnimation, true)) {
			return;
		}

		// draw the little animated arrow in the bottom right of the dialog box
		static boolean arrowFrame = false;
		if (arrowFrame) {
			// top arrow
			display->startWrite();
			display->fillRectangleTx(283, yPos - 4, 14, 6, BLACK);
			display->fillRectangleTx(283, yPos - 7, 14, 2, WHITE);
			display->fillRectangleTx(286, yPos - 5, 8, 2, WHITE);
			display->fillRectangleTx(289, yPos - 3, 2, 2, WHITE);
			display->endWrite();
		} else {
			// bottom arrow
			display->startWrite();
			display->fillRectangleTx(283, yPos - 7, 14, 6, BLACK);
			display->fillRectangleTx(283, yPos - 4, 14, 2, WHITE);
			display->fillRectangleTx(286, yPos - 2, 8, 2, WHITE);
			display->fillRectangleTx(289, yPos, 2, 2, WHITE);
			display->endWrite();
		}
		arrowFrame = !arrowFrame;
	}

	// Get the y position of the dialog box depending on alignment [top / middle / bottom]
	uint8_t getDialogYPosition(uint8_t alignment) {
		switch (alignment) {
			case dialogTop:
				return 12;
			case dialogMiddle:
				return 90;
			case dialogBottom:
			default:
				return 168;
		}
	}

	void animateRoomWhileDialogOpen(uint8_t alignment) {
		// don't start a new screen animate sequence until 500ms since last wipe
		if (drawCoords.x == 0 and drawCoords.y == 0) {
			if (!checkTimer(500, AsyncPlayModeAnimation, true)) {
				return; // not ready for animation
			}
		}

		switch (alignment) {
			case dialogTop:
				if (drawCoords.y <= 2) {
					// skip over the dialog box in the top
					drawCoords.y = 3;
					return;
				}
				break;
			case dialogBottom:
				if (drawCoords.y >= 7) {
					// skip over the dialog box in the bottom
					drawCoords.y = 0;
					return;
				}
				break;
			case dialogMiddle:
				if (drawCoords.y >= 4 and drawCoords.y <= 6) {
					// skip over the dialog box in the middle
					drawCoords.y = 7;
					return;
				}
				break;
		}

		drawRoomTileAsync(playerCoords.roomId, false);
	}

	void forceRoomFullDraw(uint8_t roomId) {
		drawCoords.x = drawCoords.y = 0;
		for (uint8_t i = 0; i < 130; i++) {
			Sound::buffer();
			drawRoomTileAsync(roomId, true);
		}
	}

	void drawDialogPagePlayMode(Dialog* d, uint8_t page, uint8_t xPos, uint8_t yPos, uint8_t scale) {
		drawDialogBox(d, page, xPos, yPos, scale);
		const uint16_t yOffset = dialogBoxOuterHeight * scale;
		const uint16_t boxWidth = dialogBoxOuterWidth * scale;
		display->fillRectangle(xPos, yPos + yOffset, boxWidth, 4, BLACK); // arrow area
	}


	/// Show a dialog box, taking input as progression through the boxes until it's finished
	/// \param dialogId dialog to show
	void showDialogSequence(uint8_t dialogId, uint8_t alignment, boolean drawBehind) {
		LEAK(F("showDialogSequence start"));
		Dialog dialog(dialogId);
		dialog.load();
		constexpr uint8_t dialogXOffset = 12;
		const uint8_t scale = 2;
		const uint8_t yPos = getDialogYPosition(alignment);
		const uint8_t pageCount = dialog.boxCount;

		for (uint8_t page = 0; page < pageCount; page++) {
			drawDialogPagePlayMode(&dialog, page, dialogXOffset, yPos, scale);

			resetTimer(ButtonMoveDelay);
			touch->clearQueue();

			// wait for input
			do {
				Sound::buffer();
				drawAnimatedDialogArrow(yPos + dialogBoxOuterHeight * scale);
				if (drawBehind) {
					animateRoomWhileDialogOpen(alignment);
				}

				// Check if we need to exit to the main menu
				static uint8_t loopCounter = 0;
				// only check every 15th cpu cycle
				if (loopCounter++ == 15) {
					loopCounter = 0;

					touch->poll();
					if (!checkTimer(250, BetweenTouches) or !touch->isPressed()) {
						continue;
					}

					DEBUG(F("opened exit prompt"));

					if (exitingPlayMode()) {
						Sound::stop();
						state = MainMenuState;
						deallocate();
						return;
					}

					// redraw room and the current dialog box if we don't return to menu
					forceRoomFullDraw(playerCoords.roomId);
					drawDialogPagePlayMode(&dialog, page, dialogXOffset, yPos, scale);
				}

			} while (pollInput() == no_change);
		}

		touch->clearQueue();
		LEAK(F("showDialogSequence end"));
	}

	boolean isPlayerCoordinates(Coordinates* c) {
		return c->fullEqual(playerCoords);
	}

	/// Checks if a GameObject is in an updated state due to flags
	/// \param g GameObject to check
	/// \return whether the GameObject is in an updated state
	boolean objectInUpdatedState(GameObject* g) {
		return (flag[g->updateOnFlagId()] == g->updateOnFlagSetTo());
	}

	/// Simple returns the game object at a specific coordinate in a room
	/// This function should respect all flag update rules, dynamic objects, displacements, etc.
	GameObject getObjectAtCoord(Coordinates position, boolean considerDynamic,
										 boolean considerPlayer) {
		// get the object at a specific location
		GameObject g(_NO_GAMEOBJECT);

		if (considerPlayer) {
			if (isPlayerCoordinates(&position)) {
				g = playerGameObj;
				g.load(); // todo is this necessary? added after adding dynamic, before didnt have it
				displaceObject(&g);
				return g;
			}
		}

		if (considerDynamic) {
			// handles when there's an object moved to this position
			if (isDynamicObjectAtPosition(position)) {
				g.id = dynamicGameObjectIdAtCoords(position);
				g.load();
				displaceObject(&g);
				return g;
			}

			// handles when a dynamic object has left its original position
			// important: needs to go after isDynamicObjectAtPosition since it won't consider if
			// there's a new object at the original position of another dyn obj
			if (isDynamicObjectOriginButItMoved(position)) {
				g.id = _NO_GAMEOBJECT;
				g.load();
				return g;
			}
		}

		// handle default case where we simply load the object in the room
		RoomHelper::getGameObjectId(&g.id, position);
		g.load();
		displaceObject(&g);
		return g;
	}

	/// Draws a single object in the room, if it's the last object, it switches the frame
	/// \param roomId room to draw
	/// \param fullDraw if true, the entire screen will be redrawn. If false, only the changed tiles
	/// will be redrawn \param showingDialog true if the dialog box is currently being displayed
	void drawRoomTileAsync(uint8_t roomId, boolean fullDraw) {
		Coordinates position(drawCoords.x, drawCoords.y, roomId);

		// get the object at a specific location
		GameObject g(_NO_GAMEOBJECT);
		g = getObjectAtCoord(position);

		const boolean frame = getFrameToDraw(&g, currentFrame);
		drawGameObjectAtCoords(&g, &roomPalette, &position, 3, frame, fullDraw);

		drawCoords.x++;

		// done with row
		if (drawCoords.x == 13) {
			drawCoords.x = 0;
			drawCoords.y++;
		}

		// done drawing the screen
		if (drawCoords.y == 10) {
			drawCoords.x = 0;
			drawCoords.y = 0;
			currentFrame = !currentFrame;
		}
	}

	///////////////////////
	/// DEBUG FUNCTIONS ///
	///////////////////////
	void stateOfPlay() {
#ifdef SHOW_INFO
		Serial << "=========== stateOfPlay() ===========" << endl;
		Serial << "Current room: " << playerCoords.roomId << endl;
		Serial << "New room: " << newCoords.roomId << endl;
		Serial << "Draw coordinates: (" << drawCoords.x << ", " << drawCoords.y << ")" << endl;
		Serial << "Current coordinates: (" << playerCoords.x << ", " << playerCoords.y << ")" << endl;
		Serial << "New coordinates: (" << newCoords.x << ", " << newCoords.y << ")" << endl;
		Serial << "Current frame: " << ((currentFrame == first_frame) ? "1" : "2") << endl;
		Serial << "=====================================" << endl;
#endif
	}
} // namespace Play