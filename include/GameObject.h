#ifndef GameObject_h
#define GameObject_h

#include <Arduino.h>
#include "globals/Typedefs.h"
#include "globals/Bitwise.h"
#include "RamBlock.h"
#include "spi/ExtEeprom.h"
#include "Coordinates.h"
#include "globals/SerialDebugging.h"
#include "globals/DataBounds.h"

class GameObject {
	public:
		Object data;
		uint16_t id = 0;

		explicit GameObject(u16 gameObjectId) : id(gameObjectId) {
			if (id >= objectCount) {
				ERROR(F("gameobject OOB"));
				id = _NO_GAMEOBJECT;
			}
		}

		// Load from memory
		void load(boolean fromRam = true);
		void save();

		void toString();
		Coordinates getTeleportLocation();
		void reset();

		// Prior memory location + PREVIOUS memory size
		def u8 SPRITE_COLOR_PALETTE = 0;
		def u8 SPRITE_IS_ANIMATED = (SPRITE_COLOR_PALETTE + VALUES_2);
		def u8 SPRITE_STARTS_ON_FRAME_TWO = (SPRITE_IS_ANIMATED + VALUES_2);

		def u8 TOUCH_IS_SOLID = (SPRITE_STARTS_ON_FRAME_TWO + VALUES_2);
		// instead of storing "does not trigger" and the flag id, could just store no flag to
		// represent not triggering a flag, but then the flag gets cleared when you toggle it on and
		// off in the editor
		def u8 TOUCH_TRIGGERS_FLAG = (TOUCH_IS_SOLID + VALUES_2);
		def u8 TOUCH_TRIGGERS_FLAG_ID = (TOUCH_TRIGGERS_FLAG + VALUES_2);
		def u8 TOUCH_FLIPS_FLAG = (TOUCH_TRIGGERS_FLAG_ID + flagIdSize);
		def u8 TOUCH_SETS_FLAG_TO = (TOUCH_FLIPS_FLAG + VALUES_2);

		def u8 TOUCH_TELEPORTS_TO_ROOM = (TOUCH_SETS_FLAG_TO + VALUES_2);
		def u8 TOUCH_TELEPORTS_TO_ROOM_ID = (TOUCH_TELEPORTS_TO_ROOM + VALUES_2);
		def u8 TOUCH_TELEPORTS_TO_ROOM_X = (TOUCH_TELEPORTS_TO_ROOM_ID + roomIdSize);
		def u8 TOUCH_TELEPORTS_TO_ROOM_Y = (TOUCH_TELEPORTS_TO_ROOM_X + VALUES_16);
		def u8 TOUCH_TELEPORT_REQUIRES_FLAG = (TOUCH_TELEPORTS_TO_ROOM_Y + VALUES_16);
		def u8 TOUCH_TELEPORT_REQUIRES_FLAG_ID = (TOUCH_TELEPORT_REQUIRES_FLAG + VALUES_2);
		def u8 TOUCH_TELEPORT_REQUIRES_FLAG_SET_TO = (TOUCH_TELEPORT_REQUIRES_FLAG_ID + flagIdSize);

		def u8 TOUCH_SHOWS_DIALOG = (TOUCH_TELEPORT_REQUIRES_FLAG_SET_TO + VALUES_2);
		def u8 TOUCH_SHOWS_DIALOG_ID = (TOUCH_SHOWS_DIALOG + VALUES_2);
		def u8 TOUCH_SHOWS_DIALOG_REQUIRES_FLAG = (TOUCH_SHOWS_DIALOG_ID + dialogIdSize);
		def u8 TOUCH_SHOWS_DIALOG_REQUIRES_FLAG_ID = (TOUCH_SHOWS_DIALOG_REQUIRES_FLAG + VALUES_2);
		def u8 TOUCH_SHOWS_DIALOG_REQUIRES_FLAG_SET_TO =
				  (TOUCH_SHOWS_DIALOG_REQUIRES_FLAG_ID + flagIdSize);

		def u8 UPDATE_ON_FLAG_ID = (TOUCH_SHOWS_DIALOG_REQUIRES_FLAG_SET_TO + VALUES_2);
		def u8 UPDATE_ON_FLAG_SET_TO = (UPDATE_ON_FLAG_ID + flagIdSize);
		def u8 UPDATE_SETS_SOLID = (UPDATE_ON_FLAG_SET_TO + VALUES_2);
		def u8 UPDATE_SETS_SOLID_TO = (UPDATE_SETS_SOLID + VALUES_2);
		def u8 UPDATE_AFFECTS_FRAME = (UPDATE_SETS_SOLID_TO + VALUES_2);
		def u8 UPDATE_SETS_FRAME_TO = (UPDATE_AFFECTS_FRAME + VALUES_2);
		def u8 UPDATE_AFFECTS_ANIMATED = (UPDATE_SETS_FRAME_TO + VALUES_2);
		def u8 UPDATE_SETS_ANIMATED_TO = (UPDATE_AFFECTS_ANIMATED + VALUES_2);
		def u8 UPDATE_DISPLACES = (UPDATE_SETS_ANIMATED_TO + VALUES_2);
		def u8 UPDATE_DISPLACES_WITH_ID = (UPDATE_DISPLACES + VALUES_2);

		def u8 INITIAL_MOVEMENT_TYPE = (UPDATE_DISPLACES_WITH_ID + objectIdSize);
		def u8 UPDATE_AFFECTS_MOVEMENT_TYPE = (INITIAL_MOVEMENT_TYPE + VALUES_4);
		def u8 UPDATE_SETS_MOVEMENT_TYPE_TO = (UPDATE_AFFECTS_MOVEMENT_TYPE + VALUES_2);

		def u8 LOGIC_SIZE = (UPDATE_SETS_MOVEMENT_TYPE_TO + VALUES_4);

		/// When drawing we use a color palette with three colors (background, foreground, and
		/// highlight) \return true to use highlight and background, false to use foreground and
		/// background
		boo isHighlighted() {
			return testBit(data.logic, SPRITE_COLOR_PALETTE);
		}

		/// Does the GameObject start with an initial state of animated?
		/// Even if the object isAnimated, it may not be animated after some later flag update!
		/// \return true if the sprite is initially animated
		boolean isAnimated() {
			return testBit(data.logic, SPRITE_IS_ANIMATED);
		}

		/// Does the GameObject start with an initial state of frame 2?
		/// Even if the object starts on frame 2, it may be flipped by a flag update
		/// This also may affect which frame is drawn on which animation cycle (todo: check)
		/// \return true if the sprite is initially on frame 2
		boolean startsOnFrameTwo() {
			return testBit(data.logic, SPRITE_STARTS_ON_FRAME_TWO);
		}

		/// What is the movement type of this object? Options: [target player / wander / still]
		u8 initialMovementType() {
			const u8 value = getBits(data.logic, INITIAL_MOVEMENT_TYPE, VALUES_4);
			return value;
		}

		/// Is the object treated as initially impassable?
		/// Objects can trigger flag updates whether or not they are solid
		/// Can be changed by a flag update though
		/// \return true if the object is initially solid
		boolean isSolid() {
			return testBit(data.logic, TOUCH_IS_SOLID);
		}

		/// Does the object trigger a flag update when touched?
		/// Flag updates can flip or set a flag's value
		/// For which flag this object updates, check touchTriggersFlagId()
		/// \return true if the object triggers a flag update
		boolean touchTriggersFlag() {
			return testBit(data.logic, TOUCH_TRIGGERS_FLAG);
		}

		/// What flag does the object modify when touched?
		/// When a flag gets updated, we redraw the screen since objects may change
		/// Objects can have their frame change, be set to solid/not solid,
		/// or even be replaced with another object entirely
		/// \return the flag id
		u8 touchTriggersFlagId() {
			const u8 flagId = getBits(data.logic, TOUCH_TRIGGERS_FLAG_ID, flagIdSize);
			if (flagId >= flagCount) {
				return _NO_FLAG;
			}
			return flagId;
		}

		/// Does the object flip the value of the flag it triggers?
		/// If not, it sets the flag to the value specified by touchSetsFlagTo()
		/// \return true if the flag is flipped, false if it is set to a value
		boolean touchFlipsFlag() {
			return testBit(data.logic, TOUCH_FLIPS_FLAG);
		}

		/// What value does the object set the flag to when touched?
		/// Only used if touchFlipsFlag() is false
		/// \return the value to set the flag to
		boolean touchSetsFlagTo() {
			return testBit(data.logic, TOUCH_SETS_FLAG_TO);
		}

		/// Does the object potentially teleport the player to a new room when touched?
		/// If so, the player will be moved to the room specified by touchTeleportsToRoomId()
		/// Dependent on whether a flag is required to teleport or not by touchTeleportRequiresFlag()
		/// \return true if the player can be potentially teleported
		boolean touchTeleportsToRoom() {
			return testBit(data.logic, TOUCH_TELEPORTS_TO_ROOM);
		}

		/// What room does the object teleport the player to when touched?
		/// Only used if touchTeleportsToRoom() is true
		/// \return the room id
		u8 touchTeleportsToRoomId() {
			const u8 roomId = getBits(data.logic, TOUCH_TELEPORTS_TO_ROOM_ID, roomIdSize);
			if (roomId > roomCount) {
				ERROR(F("touchTeleportsToRoomId OOB: ") << roomId);
				return _NO_ROOM;
			}
			return roomId;
		}

		/// Where in the room does the object teleport the player to?
		/// Only used if touchTeleportsToRoom() is true
		/// \return the x coordinate in the room
		u8 touchTeleportsToRoomX() {
			const u8 xValue = getBits(data.logic, TOUCH_TELEPORTS_TO_ROOM_X, VALUES_16);
			if (xValue >= 13) {
				return 0;
			}
			return xValue;
		}

		/// Where in the room does the object teleport the player to?
		/// Only used if touchTeleportsToRoom() is true
		/// \return the y coordinate in the room
		u8 touchTeleportsToRoomY() {
			const u8 yValue = getBits(data.logic, TOUCH_TELEPORTS_TO_ROOM_Y, VALUES_16);
			if (yValue >= 10) {
				return 0;
			}
			return yValue;
		}

		/// Teleportation may only occur when a certain flag is set to a certain value
		/// This is the flag id that must be set to the value specified by
		/// touchTeleportsToRoomRequiresFlagSetTo() \return whether or not teleportation requires a
		/// flag
		boolean touchTeleportRequiresFlag() {
			return testBit(data.logic, TOUCH_TELEPORT_REQUIRES_FLAG);
		}

		/// Teleportation may only occur when this flag is set to a certain value
		/// \return the flag id
		u8 touchTeleportRequiresFlagId() {
			const u8 flagId = getBits(data.logic, TOUCH_TELEPORT_REQUIRES_FLAG_ID, flagIdSize);
			if (flagId >= flagCount) {
				return _NO_FLAG;
			}
			return flagId;
		}

		/// Teleportation may only occur when a flag is set to this value
		/// \return the value the flag must be set to
		boolean touchTeleportRequiresFlagSetTo() {
			return testBit(data.logic, TOUCH_TELEPORT_REQUIRES_FLAG_SET_TO);
		}

		/// Does the object potentially show a dialog box when touched?
		/// May require a flag set to a certain value to show the dialog box
		/// \return true if the object potentially shows a dialog box
		boolean touchShowsDialog() {
			return testBit(data.logic, TOUCH_SHOWS_DIALOG);
		}

		/// If the object shows a dialog box, what is the dialog box id?
		/// \return the dialog box id
		u8 touchDialogId() {
			const u8 dialogId = getBits(data.logic, TOUCH_SHOWS_DIALOG_ID, dialogIdSize);
			if (dialogId >= dialogCount) {
				return _NO_DIALOG;
			}
			return dialogId;
		}

		/// Does the object require a flag to show a dialog box?
		/// \return true if the object requires a flag to show a dialog box
		boolean dialogRequiresFlag() {
			return testBit(data.logic, TOUCH_SHOWS_DIALOG_REQUIRES_FLAG);
		}

		/// If the object requires a flag to show a dialog box, what is the flag id?
		/// \return the flag id
		u8 dialogRequiresFlagId() {
			const u8 flagId = getBits(data.logic, TOUCH_SHOWS_DIALOG_REQUIRES_FLAG_ID, flagIdSize);
			if (flagId >= flagCount) {
				return _NO_FLAG;
			}
			return flagId;
		}

		/// If the object requires a flag to show a dialog box, what value must the flag be set to?
		/// \return the value the flag must be set to
		boolean dialogRequiresFlagSetTo() {
			return testBit(data.logic, TOUCH_SHOWS_DIALOG_REQUIRES_FLAG_SET_TO);
		}

		/// Does this object receive an update when the player updates some game state flag?
		/// May only trigger if the required flag is set to a certain value
		/// An update may affect solidity, initial frame, or even displace this object with another
		/// object \return true if the object receives an update
		u8 updateOnFlagId() {
			const u8 flagId = getBits(data.logic, UPDATE_ON_FLAG_ID, flagIdSize);
			if (flagId >= flagCount) {
				return _NO_FLAG;
			}
			return flagId;
		}

		/// Does this object receive an update when the player updates the required flag to this
		/// value? \return the value the flag must be set to in order to trigger an update
		boolean updateOnFlagSetTo() {
			return testBit(data.logic, UPDATE_ON_FLAG_SET_TO);
		}

		/// Does this object change solidity when the player updates the required flag?
		/// \return true if the object changes solidity
		boolean updateAffectsSolidity() {
			return testBit(data.logic, UPDATE_SETS_SOLID);
		}

		/// Does this object change to solid or not solid when the player updates the required flag?
		/// \return true if the object changes to solid
		boolean updateSetsSolidityTo() {
			return testBit(data.logic, UPDATE_SETS_SOLID_TO);
		}

		/// Does this object change initial frame when the player updates the required flag?
		/// \return true if the object changes initial frame
		boolean updateAffectsFrame() {
			return testBit(data.logic, UPDATE_AFFECTS_FRAME);
		}

		/// What is the new initial frame when the player updates the required flag?
		/// \return the new initial frame
		boolean updateSetsFrameTo() {
			return testBit(data.logic, UPDATE_SETS_FRAME_TO);
		}

		/// Does the object change from animated to not animated or vice versa on update?
		/// \return true if the object's animation status is affected by an update
		boolean updateAffectsAnimated() {
			return testBit(data.logic, UPDATE_AFFECTS_ANIMATED);
		}

		/// If the object's animation status is affected by an update, does it become animated?
		/// \return true if the object should become animated after an update
		boolean updateSetsAnimatedTo() {
			return testBit(data.logic, UPDATE_SETS_ANIMATED_TO);
		}

		/// Does this object change into another object when the player updates the required flag?
		/// \return true if the object changes into another object
		boolean updateReplacesWithGameObject() {
			return testBit(data.logic, UPDATE_DISPLACES);
		}

		/// What is the new object to use as this object when the player updates the required flag?
		/// \return the new object id
		uint16_t updateReplacesWithGameObjectId() {
			const uint16_t objectId = getBits(data.logic, UPDATE_DISPLACES_WITH_ID, objectIdSize);
			if (objectId >= objectCount) {
				return _NO_GAMEOBJECT;
			}
			return objectId;
		}

		boolean updateAffectsMovement() {
			return testBit(data.logic, UPDATE_AFFECTS_MOVEMENT_TYPE);
		}

		u8 updateSetsMovementTo() {
			return getBits(data.logic, UPDATE_SETS_MOVEMENT_TYPE_TO, VALUES_4);
		}

		// Setters
		void set_IsHighlighted(boolean value) {
			setBit(data.logic, SPRITE_COLOR_PALETTE, value);
		}

		void set_isAnimated(boolean value) {
			setBit(data.logic, SPRITE_IS_ANIMATED, value);
		} // is the object animated?

		void set_startsOnFrameTwo(boolean value) {
			setBit(data.logic, SPRITE_STARTS_ON_FRAME_TWO, value);
		} // if the object isn't animated, what frame should it be on?

		void set_initialMovementType(u8 value) {
			// INFO(F("set_initialMovementType: ") << value);
			setBits(data.logic, INITIAL_MOVEMENT_TYPE, VALUES_4, value);
		}

		void set_isSolid(boolean value) {
			setBit(data.logic, TOUCH_IS_SOLID, value);
		} // is the object solid and does it trigger collision code?

		void set_touchTriggersFlag(boolean value) {
			setBit(data.logic, TOUCH_TRIGGERS_FLAG, value);
		}

		void set_touchTriggersFlagId(u8 value) {
			setBits(data.logic, TOUCH_TRIGGERS_FLAG_ID, flagIdSize, value);
		}

		void set_touchFlipsFlag(boolean value) {
			setBit(data.logic, TOUCH_FLIPS_FLAG, value);
		}

		void set_touchSetsFlagTo(boolean value) {
			setBit(data.logic, TOUCH_SETS_FLAG_TO, value);
		}

		void set_touchTeleportsToRoom(boolean value) {
			setBit(data.logic, TOUCH_TELEPORTS_TO_ROOM, value);
		}

		void set_touchTeleportsToRoomId(u8 value) {
			setBits(data.logic, TOUCH_TELEPORTS_TO_ROOM_ID, roomIdSize, value);
		}

		void set_touchTeleportsToRoomX(u8 value) {
			setBits(data.logic, TOUCH_TELEPORTS_TO_ROOM_X, VALUES_16, value);
		}

		void set_touchTeleportsToRoomY(u8 value) {
			setBits(data.logic, TOUCH_TELEPORTS_TO_ROOM_Y, VALUES_16, value);
		}

		void set_touchTeleportRequiresFlag(boolean value) {
			setBit(data.logic, TOUCH_TELEPORT_REQUIRES_FLAG, value);
		}

		void set_touchTeleportRequiresFlagId(u8 value) {
			setBits(data.logic, TOUCH_TELEPORT_REQUIRES_FLAG_ID, flagIdSize, value);
		}

		void set_touchTeleportRequiresFlagSetTo(boolean value) {
			setBit(data.logic, TOUCH_TELEPORT_REQUIRES_FLAG_SET_TO, value);
		}

		void set_touchShowsDialog(boolean value) {
			setBit(data.logic, TOUCH_SHOWS_DIALOG, value);
		}

		void set_touchShowsDialogId(u8 value) {
			setBits(data.logic, TOUCH_SHOWS_DIALOG_ID, dialogIdSize, value);
		}

		void set_touchShowsDialogRequiresFlag(boolean value) {
			setBit(data.logic, TOUCH_SHOWS_DIALOG_REQUIRES_FLAG, value);
		}

		void set_touchShowsDialogRequiresFlagId(u8 value) {
			setBits(data.logic, TOUCH_SHOWS_DIALOG_REQUIRES_FLAG_ID, flagIdSize, value);
		}

		void set_touchShowsDialogRequiresFlagSetTo(boolean value) {
			setBit(data.logic, TOUCH_SHOWS_DIALOG_REQUIRES_FLAG_SET_TO, value);
		}

		void set_updateOnFlagId(u8 value) {
			setBits(data.logic, UPDATE_ON_FLAG_ID, flagIdSize, value);
		}

		void set_updateOnFlagSetTo(boolean value) {
			setBit(data.logic, UPDATE_ON_FLAG_SET_TO, value);
		}

		void set_updateAffectsSolidity(boolean value) {
			setBit(data.logic, UPDATE_SETS_SOLID, value);
		}

		void set_updateSetsSolidityTo(boolean value) {
			setBit(data.logic, UPDATE_SETS_SOLID_TO, value);
		}

		void set_updateAffectsFrame(boolean value) {
			setBit(data.logic, UPDATE_AFFECTS_FRAME, value);
		}

		void set_updateSetsFrameTo(boolean value) {
			setBit(data.logic, UPDATE_SETS_FRAME_TO, value);
		}

		void set_updateAffectsAnimated(boolean value) {
			setBit(data.logic, UPDATE_AFFECTS_ANIMATED, value);
		}

		void set_updateSetsAnimatedTo(boolean value) {
			setBit(data.logic, UPDATE_SETS_ANIMATED_TO, value);
		}

		void set_updateReplacesWithGameObject(boolean value) {
			setBit(data.logic, UPDATE_DISPLACES, value);
		}

		void set_updateReplacesWithGameObjectId(u8 value) {
			setBits(data.logic, UPDATE_DISPLACES_WITH_ID, objectIdSize, value);
		}

		void set_updateAffectsMovementType(boolean value) {
			setBit(data.logic, UPDATE_AFFECTS_MOVEMENT_TYPE, value);
		}

		void set_updateSetsMovementTypeTo(u8 value) {
			setBits(data.logic, UPDATE_SETS_MOVEMENT_TYPE_TO, VALUES_4, value);
		}
};

#endif
