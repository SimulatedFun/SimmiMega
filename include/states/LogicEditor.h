#ifndef LogicEditorState_h
#define LogicEditorState_h

#include <Arduino.h>
#include "globals/DataBounds.h"
#include "globals/Async.h"
#include "states/states.h"
#include "Dialog.h"
#include "GameObject.h"
#include "DrawBMP.h"
#include "ui/UIElement.h"
#include "ui/NavigationElement.h"
#include "ui/ValueButton.h"
#include "ui/RoundButton.h"
#include "ui/LogicEditorElements.h"
#include "ui/DropdownFlagUpdate.h"
#include "ui/Checkbox.h"
#include "ui/windows/ChooseFlag.h"
#include "ui/DropdownTeleport.h"
#include "ui/windows/ChooseXY.h"
#include "ui/DropdownDialog.h"
#include "ui/windows/ChooseDialog.h"
#include "ui/windows/ChooseObject.h"
#include "ui/windows/ChooseRoom.h"

// Make sure when opening a window for a selection to call
// touch.clearQueue();
// so the press isn't processed underneath their selection

namespace LogicEditor {
	// done ish
	void setup();
	void createPersistentUI();
	void deallocatePersistentUI();
	void drawVisualTab();
	void drawCollisionTab();
	void drawAdvancedTab();

	void callbackChangeTab(LogicTab);
	void callbackExit(RoundButton&);

	void callbackChangeObject(boolean);
	void callbackCheckAnimated(boolean);
	void callbackCheckSolidity(boolean);
	void callbackCheckStartingFrame(boolean);
	void callbackCheckDisplace(boolean);
	void callbackCheckMovement(boolean);

	void callbackDropdownUpdateFlag(boolean);
	void callbackDropDownUpdateFlagTo(boolean);
	void callbackCloseDropdown(boolean);

	void callbackUpdateOnFlag(boolean);
	void callbackUpdateOnFlagSetTo(boolean);
	void callbackChangeAnimatedToOnUpdate(boolean);
	void callbackChangeSolidityOnUpdate(boolean);
	void callbackUpdateStartingFrameOnUpdate(boolean);

	void callbackChangeAnimated(boolean);
	void callbackChangeStartingFrame(boolean);
	void callbackChangeSolidity(boolean);
	void callbackChangeDisplaceObjOnUpdate(boolean);

	void callbackTeleportRequiredFlag(boolean);
	void callbackTeleportFlagSetTo(boolean);
	void callbackTeleportChooseRoom(boolean);
	void callbackTeleportChooseXY(boolean);

	void callbackSelectDialog(boolean);
	void callbackDialogFlagRequired(boolean);
	void callbackDialogFlagSetTo(boolean);

	void callbackChangeInitialMovementType(uint8_t movementType);
	void callbackUpdateChangesMovementType(uint8_t movementType);

	void deallocate();

	void createVisualTabUI();
	void deallocateVisualTabUI();

	void createCollisionTabUI();
	void registerCollisionTabUI();
	void deallocateCollisionTabUI();

	void createAdvancedTabUI();
	void deallocateAdvancedTabUI();

	void initialDraw();
	void drawGameObjectId();

	void drawPersistentUI();

	void loop();
} // namespace LogicEditor
#endif
