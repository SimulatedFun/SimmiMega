#include "states/LogicEditor.h"

/// Loads a gameobject from external memory
/// makes changes to it in ram based on the UI states in the logic editor
/// then saves back to external mem when you exit
namespace LogicEditor {
	LogicTab currentTab;
	GameObject* gameObject;

	// Persistent UI elements across all three tabs
	RoundButton* exit;
	ObjectSelector* objPreview;
	WordTabElement *visualTab, *collisionTab, *advancedTab;

	// Visual Tab
	AnimatedSelection* animatedInitially; // whether the sprite is initially animated
	OneTwoSelection* startingFrame;		  // whether the sprite starts on frame 1 or 2 initially
	MovementSelection* movementInitial;

	// Collision Tab
	SoliditySelection* solidity;				 // whether this object is solid initially
	DropdownFlagUpdate* dropdownFlagUpdate; // dropdown for how a touch will update a flag
	DropdownTeleport* dropdownTeleport;
	DropdownDialog* dropdownDialog;

	// Advanced Tab
	Checkbox *checkChangesAnimated, *checkChangesSolidity, *checkChangesStartingFrame,
			  *checkDisplaces, *checkChangesMovement;
	FlagSelector* updateOnFlag; // only trigger this object's update if this flag is modified
	TrueFalseSelection*
			  updateOnFlagSetTo; // only trigger this object's update if the flag is set to this
	AnimatedSelection* updateAnimated;			  // change to animated or not on update
	SoliditySelection* updateSolidity;			  // change to this solidity on update
	OneTwoSelection* updateStartingFrame;		  // change starting frame to this on update
	ObjectIdSelector* displaceObjectIDSelector; // displace with this gameobject on update
	MovementSelection* updateChangesMovement;	  // update movement type to this

	/// Sets up, allocates memory, draws, etc. for the logic editor
	/// Loop() is called right after this function by the state machine
	void setup() {
		LEAK(F("enter logic edit"));
		const uint8_t gameObjectId = ChooseObject::pick();
		// if no obj is picked, return to main menu
		if (gameObjectId == _NO_GAMEOBJECT) {
			state = MainMenuState;
			return;
		}

		// Load in game object information to populate logic editor
		gameObject = new GameObject(gameObjectId);
		gameObject->load(); // load everything
		gameObject->toString();

		UIHelper::clearActive();
		createPersistentUI();
		createVisualTabUI();
		currentTab = VisualTab;
		visualTab->selected = true;

		touch->clearQueue(); // do before draw so you can touch faster
		initialDraw();
	}

	/// Just loops UI element touch checking
	void loop() {
		UIHelper::loop();
	}

	// region Draw functions

	/// Redraws the logic editor fully based on the current tab selection
	void initialDraw() {
		switch (currentTab) {
			case VisualTab:
				drawVisualTab();
				break;
			case CollisionTab:
				drawCollisionTab();
				break;
			case AdvancedTab:
				drawAdvancedTab();
				break;
			case ProjectSettingsTab:
			case SystemSettingsTab:
				break;
		}
		drawPersistentUI();
	}

	/// Draws the bottom nav and top right exit/preview ui
	/// Does not draw anything in the content area
	void drawPersistentUI() {
		exit->render();
		display->fillRectangle(288, 24, 24, 217, RGB565(0x413650));
		drawGameObjectId();
		objPreview->render();

		// separator above the tabs
		display->startWrite();
		{
			display->drawHorizontalLineTx(0, 218, 312, RGB565(0x937ac5));
			display->drawHorizontalLineTx(0, 217, 312, RGB565(0x2d1b2e));
		}
		display->endWrite();

		visualTab->render();
		collisionTab->render();
		advancedTab->render();
	}

	/// Draws the current game object ID in the top right (under exit button)
	void drawGameObjectId() {
		Text* id = new Text(gameObject->id);
		const uint8_t width = 24;
		id->setPosition(288 + id->getPaddingToCenter(width), 38);
		id->setStyle(1, RGB565(0x937ac5));
		id->print();
		delete id;
	}

	/// Draws the content area of the visualTab tab
	void drawVisualTab() {
		display->fillRectangle(0, 0, 288, 217, RGB565(0x413650));

		DrawingUtils::simplePrint(16, 28, F("Animated:"), WHITE, 1);
		animatedInitially->animated = gameObject->isAnimated();
		animatedInitially->render();

		DrawingUtils::simplePrint(16, 53, F("Starting Frame:"), WHITE, 1);
		if (gameObject->startsOnFrameTwo()) {
			startingFrame->frame = second_frame;
		} else {
			startingFrame->frame = first_frame;
		}
		startingFrame->render();

		DrawingUtils::simplePrint(16, 78, F("Movement:"), WHITE, 1);
		movementInitial->type = gameObject->initialMovementType();
		movementInitial->render();
	}

	/// Draws the contents of the collision tab
	void drawCollisionTab() {
		display->fillRectangle(0, 0, 288, 217, RGB565(0x413650));
		DrawingUtils::simplePrint(16, 28, F("Solidity:"), WHITE, 1);
		DrawingUtils::simplePrint(16, 53, F("On Collision:"), WHITE, 1);

		// white lines
		display->startWrite();
		{
			display->drawVerticalLineTx(18, 62, 116, WHITE);
			display->drawHorizontalLineTx(19, 74, 5, WHITE);
			display->drawHorizontalLineTx(19, 121, 5, WHITE);
			display->drawHorizontalLineTx(19, 177, 5, WHITE);
		}
		display->endWrite();

		solidity->render();
		dropdownFlagUpdate->render();

		if (!gameObject->touchTeleportsToRoom() or !gameObject->touchTeleportRequiresFlag()) {
			dropdownTeleport->setPosition(25, 108);
		} else if (gameObject->touchTeleportRequiresFlag()) {
			dropdownTeleport->setPosition(25, 95);
		}
		dropdownTeleport->render();

		if (!gameObject->touchShowsDialog() or !gameObject->dialogRequiresFlag()) {
			dropdownDialog->setPosition(25, 164);
		} else if (gameObject->dialogRequiresFlag()) {
			dropdownDialog->setPosition(25, 153);
		}
		dropdownDialog->render();
	}

	/// Draws the contents of the advanced tab
	void drawAdvancedTab() {
		display->fillRectangle(0, 0, 288, 217, RGB565(0x413650));
		DrawingUtils::simplePrint(11, 25, F("When"), WHITE, 1);

		updateOnFlag->setData(gameObject->updateOnFlagId());
		updateOnFlag->render();

		DrawingUtils::simplePrint(83, 25, F("is"), WHITE, 1);

		updateOnFlagSetTo->state = gameObject->updateOnFlagSetTo();
		updateOnFlagSetTo->render();

		DrawingUtils::simplePrint(129, 25, F(":"), WHITE, 1);

		// white lines
		display->startWrite();
		{
			display->drawVerticalLineTx(13, 34, 117, WHITE);
			for (uint8_t i = 0; i < 5; i++) {
				display->drawHorizontalLineTx(14, 46 + i * 26, 5, WHITE);
			}
		}
		display->endWrite();

		uint16_t textColor;

		if (gameObject->updateReplacesWithGameObject()) {
			textColor = WHITE;
			checkDisplaces->checked = true;
			displaceObjectIDSelector->enabled = true;
		} else {
			textColor = DARK_GREY;
			checkDisplaces->checked = false;
			displaceObjectIDSelector->enabled = false;
		}
		checkDisplaces->render();
		DrawingUtils::simplePrint(44, 51, F("Displace Object with"), textColor, 1);

		displaceObjectIDSelector->gameObjectId = gameObject->updateReplacesWithGameObjectId();
		displaceObjectIDSelector->render();

		//

		if (gameObject->updateAffectsAnimated()) {
			textColor = WHITE;
			checkChangesAnimated->checked = true;
			updateAnimated->enabled = true;
		} else {
			textColor = DARK_GREY;
			checkChangesAnimated->checked = false;
			updateAnimated->enabled = false;
		}
		checkChangesAnimated->render();
		DrawingUtils::simplePrint(44, 77, F("Change to"), textColor, 1);

		if (gameObject->updateSetsAnimatedTo()) {
			updateAnimated->animated = true;
		} else {
			updateAnimated->animated = false;
		}
		updateAnimated->render();

		//

		if (gameObject->updateAffectsSolidity()) {
			textColor = WHITE;
			checkChangesSolidity->checked = true;
			updateSolidity->enabled = true;
		} else {
			textColor = DARK_GREY;
			checkChangesSolidity->checked = false;
			updateSolidity->enabled = false;
		}
		checkChangesSolidity->render();
		updateSolidity->render();
		DrawingUtils::simplePrint(44, 103, F("Change to"), textColor, 1);

		//

		if (gameObject->updateAffectsFrame()) {
			textColor = WHITE;
			checkChangesStartingFrame->checked = true;
			updateStartingFrame->enabled = true;
		} else {
			textColor = DARK_GREY;
			checkChangesStartingFrame->checked = false;
			updateStartingFrame->enabled = false;
		}
		checkChangesStartingFrame->render();
		DrawingUtils::simplePrint(44, 129, F("Change Starting Frame to"), textColor, 1);
		updateStartingFrame->frame = gameObject->updateSetsFrameTo();
		updateStartingFrame->render();

		//

		if (gameObject->updateAffectsMovement()) {
			textColor = WHITE;
			checkChangesMovement->checked = true;
			updateChangesMovement->enabled = true;
		} else {
			textColor = DARK_GREY;
			checkChangesMovement->checked = false;
			updateChangesMovement->enabled = false;
		}
		checkChangesMovement->render();
		DrawingUtils::simplePrint(44, 156, F("Change Movement to"), textColor, 1);
		updateChangesMovement->type = gameObject->updateSetsMovementTo();
		updateChangesMovement->render();
	}

	// endregion

	// region Allocating and deallocating tab UI elements

	/// Allocates and registers the persistent UI elements such as the exit button, object preview,
	/// and tabs
	void createPersistentUI() {
		exit = new RoundButton(ExitButton);
		exit->setPosition(288, 0);
		exit->callback.bind(&callbackExit);
		UIHelper::registerUI(exit);

		objPreview = new ObjectSelector();
		objPreview->setPosition(292, 45);
		objPreview->callback.bind(&callbackChangeObject);
		objPreview->setData(gameObject);
		UIHelper::registerUI(objPreview);

		visualTab = new WordTabElement(VisualTab);
		visualTab->setPosition(0, 219);
		visualTab->callback.bind(&callbackChangeTab);
		UIHelper::registerUI(visualTab);

		collisionTab = new WordTabElement(CollisionTab);
		collisionTab->setPosition(104, 219);
		collisionTab->callback.bind(&callbackChangeTab);
		UIHelper::registerUI(collisionTab);

		advancedTab = new WordTabElement(AdvancedTab);
		advancedTab->setPosition(208, 219);
		advancedTab->callback.bind(&callbackChangeTab);
		UIHelper::registerUI(advancedTab);
	}

	/// Deallocates and de-registers all the persistent UI elements
	void deallocatePersistentUI() {
		UIHelper::deregisterUI(exit);
		UIHelper::deregisterUI(objPreview);
		UIHelper::deregisterUI(visualTab);
		UIHelper::deregisterUI(collisionTab);
		UIHelper::deregisterUI(advancedTab);
		delete exit;
		delete objPreview;
		delete visualTab;
		delete collisionTab;
		delete advancedTab;
	}

	/// Allocates and registers the two UI elements on the visual tab
	void createVisualTabUI() {
		animatedInitially = new AnimatedSelection();
		animatedInitially->setPosition(69, 14);
		animatedInitially->callback.bind(&callbackChangeAnimated);
		UIHelper::registerUI(animatedInitially);

		startingFrame = new OneTwoSelection();
		startingFrame->setPosition(98, 39);
		startingFrame->callback.bind(&callbackChangeStartingFrame);
		UIHelper::registerUI(startingFrame);

		movementInitial = new MovementSelection();
		movementInitial->setPosition(73, 64);
		movementInitial->callback.bind(&callbackChangeInitialMovementType);
		UIHelper::registerUI(movementInitial);
	}

	/// Deallocates and de-registers the visual tab UI elements
	void deallocateVisualTabUI() {
		UIHelper::deregisterUI(animatedInitially);
		UIHelper::deregisterUI(startingFrame);
		UIHelper::deregisterUI(movementInitial);
		delete animatedInitially;
		delete startingFrame;
		delete movementInitial;
	}

	/// Allocates and registers the collision tab UI elements
	void createCollisionTabUI() {
		solidity = new SoliditySelection();
		solidity->setPosition(63, 15);
		solidity->setSolid(gameObject->isSolid());
		solidity->callback.bind(&callbackChangeSolidity);

		dropdownFlagUpdate = new DropdownFlagUpdate(); // this also creates the sub elements
		dropdownFlagUpdate->setPosition(25, 61);
		dropdownFlagUpdate->obj = gameObject;
		dropdownFlagUpdate->touchFlag->callback.bind(&callbackDropdownUpdateFlag);
		dropdownFlagUpdate->trueFalse->callback.bind(&callbackDropDownUpdateFlagTo);
		dropdownFlagUpdate->outside->callback.bind(&callbackCloseDropdown);
		dropdownFlagUpdate->redrawFunction = &callbackCloseDropdown;

		dropdownTeleport = new DropdownTeleport();
		dropdownTeleport->obj = gameObject;
		dropdownTeleport->outside->callback.bind(&callbackCloseDropdown);
		dropdownTeleport->flagRequired->callback.bind(&callbackTeleportRequiredFlag);
		dropdownTeleport->trueFalse->callback.bind(&callbackTeleportFlagSetTo);
		dropdownTeleport->roomSelector->callback.bind(&callbackTeleportChooseRoom);
		dropdownTeleport->xySelector->callback.bind(&callbackTeleportChooseXY);
		dropdownTeleport->redrawFunction = &callbackCloseDropdown;

		dropdownDialog = new DropdownDialog();
		dropdownDialog->obj = gameObject;
		dropdownDialog->outside->callback.bind(&callbackCloseDropdown);
		dropdownDialog->dialogSelector->callback.bind(&callbackSelectDialog);
		dropdownDialog->flagRequired->callback.bind(&callbackDialogFlagRequired);
		dropdownDialog->trueFalse->callback.bind(&callbackDialogFlagSetTo);
		dropdownDialog->redrawFunction = &callbackCloseDropdown;

		registerCollisionTabUI();
	}

	/// Deallocates and de-registers the collision tab UI elements
	void deallocateCollisionTabUI() {
		UIHelper::deregisterUI(solidity);
		delete solidity;

		dropdownFlagUpdate->deregisterUI();
		UIHelper::deregisterUI(dropdownFlagUpdate);
		delete dropdownFlagUpdate;

		dropdownTeleport->deregisterUI();
		UIHelper::deregisterUI(dropdownTeleport);
		delete dropdownTeleport;

		dropdownDialog->deregisterUI();
		UIHelper::deregisterUI(dropdownDialog);
		delete dropdownDialog;
	}

	/// Allocates and registers the UI elements on the advanced tab
	void createAdvancedTabUI() {
		// When this flag ...
		updateOnFlag = new FlagSelector();
		updateOnFlag->setPosition(39, 11);
		updateOnFlag->callback.bind(&callbackUpdateOnFlag);
		UIHelper::registerUI(updateOnFlag);

		// is set to ...
		updateOnFlagSetTo = new TrueFalseSelection();
		updateOnFlagSetTo->setPosition(96, 11);
		updateOnFlagSetTo->callback.bind(&callbackUpdateOnFlagSetTo);
		UIHelper::registerUI(updateOnFlagSetTo);

		// region Checkboxes
		checkDisplaces = new Checkbox();
		checkDisplaces->setPosition(20, 37);
		checkDisplaces->callback.bind(&callbackCheckDisplace);
		UIHelper::registerUI(checkDisplaces);

		checkChangesAnimated = new Checkbox();
		checkChangesAnimated->setPosition(20, 63);
		checkChangesAnimated->callback.bind(&callbackCheckAnimated);
		UIHelper::registerUI(checkChangesAnimated);

		checkChangesSolidity = new Checkbox();
		checkChangesSolidity->setPosition(20, 89);
		checkChangesSolidity->callback.bind(&callbackCheckSolidity);
		UIHelper::registerUI(checkChangesSolidity);

		checkChangesStartingFrame = new Checkbox();
		checkChangesStartingFrame->setPosition(20, 115);
		checkChangesStartingFrame->callback.bind(&callbackCheckStartingFrame);
		UIHelper::registerUI(checkChangesStartingFrame);

		checkChangesMovement = new Checkbox();
		checkChangesMovement->setPosition(20, 141);
		checkChangesMovement->callback.bind(&callbackCheckMovement);
		UIHelper::registerUI(checkChangesMovement);
		// endregion

		displaceObjectIDSelector = new ObjectIdSelector();
		displaceObjectIDSelector->setPosition(147, 37);
		displaceObjectIDSelector->callback.bind(&callbackChangeDisplaceObjOnUpdate);
		UIHelper::registerUI(displaceObjectIDSelector);

		updateAnimated = new AnimatedSelection();
		updateAnimated->setPosition(97, 63);
		updateAnimated->callback.bind(&callbackChangeAnimatedToOnUpdate);
		UIHelper::registerUI(updateAnimated);

		updateSolidity = new SoliditySelection();
		updateSolidity->setPosition(97, 89);
		updateSolidity->callback.bind(&callbackChangeSolidityOnUpdate);
		UIHelper::registerUI(updateSolidity);

		updateStartingFrame = new OneTwoSelection();
		updateStartingFrame->setPosition(175, 115);
		updateStartingFrame->callback.bind(&callbackUpdateStartingFrameOnUpdate);
		UIHelper::registerUI(updateStartingFrame);

		updateChangesMovement = new MovementSelection();
		updateChangesMovement->setPosition(149, 141);
		updateChangesMovement->callback.bind(&callbackUpdateChangesMovementType);
		UIHelper::registerUI(updateChangesMovement);
	}

	/// Deallocates and de-registers the advanced tab UI elements
	void deallocateAdvancedTabUI() {
		UIHelper::deregisterUI(updateOnFlag);
		UIHelper::deregisterUI(updateOnFlagSetTo);
		delete updateOnFlag;
		delete updateOnFlagSetTo;

		UIHelper::deregisterUI(checkDisplaces);
		UIHelper::deregisterUI(checkChangesAnimated);
		UIHelper::deregisterUI(checkChangesSolidity);
		UIHelper::deregisterUI(checkChangesStartingFrame);
		UIHelper::deregisterUI(checkChangesMovement);
		delete checkDisplaces;
		delete checkChangesAnimated;
		delete checkChangesSolidity;
		delete checkChangesStartingFrame;
		delete checkChangesMovement;

		UIHelper::deregisterUI(displaceObjectIDSelector);
		UIHelper::deregisterUI(updateAnimated);
		UIHelper::deregisterUI(updateSolidity);
		UIHelper::deregisterUI(updateStartingFrame);
		UIHelper::deregisterUI(updateChangesMovement);
		delete displaceObjectIDSelector;
		delete updateAnimated;
		delete updateSolidity;
		delete updateStartingFrame;
		delete updateChangesMovement;
	}

	void deallocate() {
		UIHelper::clearActive();
		deallocatePersistentUI();
		switch (currentTab) {
			case VisualTab:
				deallocateVisualTabUI();
				break;
			case CollisionTab:
				deallocateCollisionTabUI();
				break;
			case AdvancedTab:
				deallocateAdvancedTabUI();
				break;
			case ProjectSettingsTab:
			case SystemSettingsTab:
				break;
		}
		delete gameObject;
	}

	// endregion

	// region Persistent UI Callbacks
	/// Binds as the callback from the exit button
	void callbackExit(RoundButton&) {
		exit->depressed = true;
		exit->render();
		gameObject->save();
		deallocate();
		state = MainMenuState;
		INFO(F("exiting the logic editor state"));
	}

	/// Handle the user using the object preview to select a new object
	void callbackChangeObject(boolean) {
		objPreview->depressed = true;
		objPreview->render();
		gameObject->save();
		deallocate();
		// hack for heap fragmentation
		oldState = MainMenuState;
		state = LogicEditorState;
	}

	/// Handles navigating from one tab to another
	/// Deallocates the old tab, creates the new tab, and renders
	void callbackChangeTab(LogicTab newTab) {
		// no change, do nothing
		if (currentTab == newTab) {
			return;
		}

		// handle deallocating the old UI elements
		// render the deselected tab UI element
		switch (currentTab) {
			case VisualTab:
				INFO(F("deallocate visual tab"));
				visualTab->selected = false;
				visualTab->render();
				deallocateVisualTabUI();
				break;
			case CollisionTab:
				INFO(F("deallocate collision tab"));
				collisionTab->selected = false;
				collisionTab->render();
				deallocateCollisionTabUI();
				break;
			case AdvancedTab:
				INFO(F("deallocate advanced tab"));
				advancedTab->selected = false;
				advancedTab->render();
				deallocateAdvancedTabUI();
				break;
			case ProjectSettingsTab:
			case SystemSettingsTab:
				break;
		}

		currentTab = newTab;

		// allocate the new tab elements
		// render the newly selected tab element
		switch (newTab) {
			case VisualTab:
				INFO(F("switch to visual tab"));
				visualTab->selected = true;
				visualTab->render();
				createVisualTabUI();
				drawVisualTab();
				break;
			case CollisionTab:
				INFO(F("switch to collision tab"));
				collisionTab->selected = true;
				collisionTab->render();
				createCollisionTabUI();
				drawCollisionTab();
				break;
			case AdvancedTab:
				INFO(F("switch to advanced tab"));
				advancedTab->selected = true;
				advancedTab->render();
				createAdvancedTabUI();
				drawAdvancedTab();
				break;
			case ProjectSettingsTab:
			case SystemSettingsTab:
				break;
		}
	}

	// endregion

	// region Visual Tab Callbacks

	/// If the object is initially animated or not
	void callbackChangeAnimated(boolean newValue) {
		gameObject->set_isAnimated(newValue);
	}

	/// If the object initially starts on frame 1 or 2
	void callbackChangeStartingFrame(boolean newValue) {
		gameObject->set_startsOnFrameTwo(newValue);
	}

	void callbackChangeInitialMovementType(uint8_t movementType) {
		gameObject->set_initialMovementType(movementType);
	}

	// endregion

	// region Collision Tab Callbacks

	/// If the object is initially solid or passthrough
	void callbackChangeSolidity(boolean inValue) {
		gameObject->set_isSolid(inValue);
	}

	// region Dropdown Flag Update
	/// Changes the flag that a collision would update
	void callbackDropdownUpdateFlag(boolean) {
		deallocatePersistentUI();
		deallocateCollisionTabUI();

		const uint8_t flagId = ChooseFlag::pick();
		if (flagId != _NO_FLAG) {
			gameObject->set_touchTriggersFlagId(flagId);
			gameObject->save();
		}

		createPersistentUI();
		createCollisionTabUI();
		collisionTab->selected = true;

		drawPersistentUI();
		drawCollisionTab();
	}

	/// Changes what value a collision sets the flag to
	void callbackDropDownUpdateFlagTo(boolean inValue) {
		gameObject->set_touchSetsFlagTo(inValue);
	}

	void registerCollisionTabUI() {
		UIHelper::clearActive();
		UIHelper::registerUI(exit);
		UIHelper::registerUI(objPreview);
		UIHelper::registerUI(visualTab);
		UIHelper::registerUI(collisionTab);
		UIHelper::registerUI(advancedTab);
		UIHelper::registerUI(solidity);

		UIHelper::registerUI(dropdownTeleport);
		dropdownTeleport->registerClosedDropdownUI();

		UIHelper::registerUI(dropdownFlagUpdate);
		dropdownFlagUpdate->registerClosedDropdownUI();

		UIHelper::registerUI(dropdownDialog);
		dropdownDialog->registerClosedDropdownUI();
	}

	/// Handles any dropdown closing
	void callbackCloseDropdown(boolean) {
		// re-register the ui elements that get cleared when we open the dropdown...
		registerCollisionTabUI();

		dropdownFlagUpdate->dropdownOpen = false;
		dropdownTeleport->dropdownOpen = false;
		dropdownDialog->dropdownOpen = false;
		collisionTab->selected = true;
		touch->clearQueue();
		initialDraw();
	}
	// endregion

	// region Dropdown Teleport
	void callbackTeleportRequiredFlag(boolean) {
		deallocatePersistentUI();
		deallocateCollisionTabUI();

		const uint8_t flagId = ChooseFlag::pick();
		if (flagId != _NO_FLAG) {
			gameObject->set_touchTeleportRequiresFlagId(flagId);
			gameObject->save();
		}

		createPersistentUI();
		createCollisionTabUI();
		collisionTab->selected = true;

		drawPersistentUI();
		drawCollisionTab();
	}

	void callbackTeleportChooseRoom(boolean) {
		deallocatePersistentUI();
		deallocateCollisionTabUI();

		const uint8_t roomId = ChooseRoom::pick();
		gameObject->set_touchTeleportsToRoomId(roomId);
		gameObject->save();

		createPersistentUI();
		createCollisionTabUI();
		collisionTab->selected = true;

		drawPersistentUI();
		drawCollisionTab();
	}

	void callbackTeleportChooseXY(boolean) {
		deallocatePersistentUI();
		deallocateCollisionTabUI();

		const Coordinates coords = ChooseXY::pick(gameObject->touchTeleportsToRoomId());
		gameObject->set_touchTeleportsToRoomX(coords.x);
		gameObject->set_touchTeleportsToRoomY(coords.y);
		gameObject->save();

		createPersistentUI();
		createCollisionTabUI();
		collisionTab->selected = true;

		drawPersistentUI();
		drawCollisionTab();
	}
	void callbackTeleportFlagSetTo(boolean newValue) {
		gameObject->set_touchTeleportRequiresFlagSetTo(newValue);
	}

	// endregion

	// region Dropdown Dialog
	void callbackDialogFlagRequired(boolean) {
		deallocatePersistentUI();
		deallocateCollisionTabUI();

		const uint8_t flagId = ChooseFlag::pick();
		if (flagId != _NO_FLAG) {
			gameObject->set_touchShowsDialogRequiresFlagId(flagId);
			gameObject->save();
		}

		createPersistentUI();
		createCollisionTabUI();
		collisionTab->selected = true;

		drawPersistentUI();
		drawCollisionTab();
	}

	void callbackDialogFlagSetTo(boolean newValue) {
		gameObject->set_touchShowsDialogRequiresFlagSetTo(newValue);
	}

	void callbackSelectDialog(boolean) {
		deallocatePersistentUI();
		deallocateCollisionTabUI();

		const uint8_t dialogId = ChooseDialog::pick();
		if (dialogId != _NO_DIALOG) {
			gameObject->set_touchShowsDialogId(dialogId);
			gameObject->save();
		}

		createPersistentUI();
		createCollisionTabUI();
		collisionTab->selected = true;

		drawPersistentUI();
		drawCollisionTab();
	}
	// endregion

	// endregion

	// region Advanced Tab Callbacks

	// When >flag< is true or false, do...
	void callbackUpdateOnFlag(boolean) {
		deallocatePersistentUI();
		deallocateAdvancedTabUI();

		const uint16_t flagId = ChooseFlag::pick();
		if (flagId != _NO_FLAG) {
			gameObject->set_updateOnFlagId(flagId);
			gameObject->save();
		}

		createPersistentUI();
		createAdvancedTabUI();
		advancedTab->selected = true;

		drawPersistentUI();
		drawAdvancedTab();
	}

	// When flag is >true or false<, do...
	void callbackUpdateOnFlagSetTo(boolean newValue) {
		gameObject->set_updateOnFlagSetTo(newValue);
	}

	// region Advanced Tab Checkbox Callbacks
	/// Handle when the user either selects or deselects the option to displace the
	/// object when an update occurs
	void callbackCheckDisplace(boolean checked) {
		{ // uncheck all the other options since displacement overrides everything
			checkChangesAnimated->checked = false;
			checkChangesAnimated->render();
			callbackCheckAnimated(false);
			//
			checkChangesSolidity->checked = false;
			checkChangesSolidity->render();
			callbackCheckSolidity(false);
			//
			checkChangesStartingFrame->checked = false;
			checkChangesStartingFrame->render();
			callbackCheckStartingFrame(false);
			//
			checkChangesMovement->checked = false;
			checkChangesMovement->render();
			callbackCheckMovement(false);
		}

		gameObject->set_updateReplacesWithGameObject(checked);

		uint16_t textColor = WHITE;
		if (checked) {
			displaceObjectIDSelector->enabled = true;
		} else {
			textColor = DARK_GREY;
			displaceObjectIDSelector->enabled = false;
		}
		DrawingUtils::simplePrint(44, 51, F("Displace Object with"), textColor, 1);
		displaceObjectIDSelector->render();
	}
	void callbackCheckAnimated(boolean checked) {
		gameObject->set_updateAffectsAnimated(checked);

		uint16_t textColor;
		if (checked) {
			textColor = WHITE;
			updateAnimated->enabled = true;
		} else {
			textColor = DARK_GREY;
			updateAnimated->enabled = false;
		}
		DrawingUtils::simplePrint(44, 77, F("Change to"), textColor, 1);
		updateAnimated->render();
	}
	void callbackCheckSolidity(boolean checked) {
		gameObject->set_updateAffectsSolidity(checked);

		uint16_t textColor;
		if (checked) {
			textColor = WHITE;
			updateSolidity->enabled = true;
		} else {
			textColor = DARK_GREY;
			updateSolidity->enabled = false;
		}
		DrawingUtils::simplePrint(44, 103, F("Change to"), textColor, 1);
		updateSolidity->render();
	}
	void callbackCheckStartingFrame(boolean checked) {
		gameObject->set_updateAffectsFrame(checked);

		uint16_t textColor;
		if (checked) {
			textColor = WHITE;
			updateStartingFrame->enabled = true;
		} else {
			textColor = DARK_GREY;
			updateStartingFrame->enabled = false;
		}
		DrawingUtils::simplePrint(44, 129, F("Change Starting Frame to"), textColor, 1);
		updateStartingFrame->render();
	}
	void callbackCheckMovement(boolean checked) {
		gameObject->set_updateAffectsMovementType(checked);

		uint16_t textColor;
		if (checked) {
			textColor = WHITE;
			updateChangesMovement->enabled = true;
		} else {
			textColor = DARK_GREY;
			updateChangesMovement->enabled = false;
		}
		DrawingUtils::simplePrint(44, 156, F("Change Movement to"), textColor, 1);
		updateChangesMovement->render();
	}
	// endregion

	/// Handle selecting a new object to displace the original object with on update
	void callbackChangeDisplaceObjOnUpdate(boolean) {
		deallocatePersistentUI();
		deallocateAdvancedTabUI();
		const uint8_t gameObjectId = ChooseObject::pick();
		gameObject->set_updateReplacesWithGameObjectId(gameObjectId);
		gameObject->save();
		createPersistentUI();
		createAdvancedTabUI();
		advancedTab->selected = true;
		drawPersistentUI();
		drawAdvancedTab();
	}
	void callbackChangeAnimatedToOnUpdate(boolean newValue) {
		gameObject->set_updateSetsAnimatedTo(newValue);
	}
	void callbackChangeSolidityOnUpdate(boolean newValue) {
		gameObject->set_updateSetsSolidityTo(newValue);
	}
	void callbackUpdateStartingFrameOnUpdate(boolean newValue) {
		gameObject->set_updateSetsFrameTo(newValue);
	}
	void callbackUpdateChangesMovementType(uint8_t type) {
		gameObject->set_updateSetsMovementTypeTo(type);
	}

	// endregion
} // namespace LogicEditor