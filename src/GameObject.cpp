#include "GameObject.h"

// todo cache frequently loaded gameobjects

void GameObject::save() {
	if (id >= objectCount) {
		ERROR(F("save gameobj OOB: ") << id);
		id = _NO_GAMEOBJECT;
	}
	if (id == _NO_GAMEOBJECT) {
		reset();
		return;
	}
	const uint16_t address = objectMemoryStart + (id - 1) * sizeof(Object);
	// INFO(F("Storing id: ") << id << ", which is " << sizeof(Object) << " bytes at address: " <<
	// address);
	eeprom->write(data, address, sizeof(Object));
	ram->write(data, address, sizeof(Object));
}

void GameObject::load(boolean fromRam) {
	if (id >= objectCount) {
		ERROR(F("load gameobj OOB: ") << id);
		id = _NO_GAMEOBJECT;
	}
	// todo is this necessary every single time? feel like it could be slow
	if (id == _NO_GAMEOBJECT) {
		reset();
		return;
	}
	const uint16_t address = objectMemoryStart + (id - 1) * sizeof(Object);
	// INFO("Loading id: " << id << ", which is " << sizeof(Object) << " bytes at address: " <<
	// address);
	if (fromRam) {
		ram->read(&data, address, sizeof(Object));
		return;
	}
	eeprom->read(&data, address, sizeof(Object));
	ram->write(data, address, sizeof(Object));
}

void GameObject::toString() {
#ifdef ENABLE_TO_STRING
#ifdef ENABLE_SERIAL
	Serial << endl;
	Serial << "+-------------------------------------------- Game Object -+" << endl;
	Serial << "| ID: ";
	print64Bit(id);
	Serial << endl;
	Serial << "+------------------------------------------------- Frames -+" << endl;
	Serial << "| ";
	print64Bit(data.frame1);
	Serial << " | ";
	print64Bit(data.frame2);
	Serial << endl;
	Serial << "+-------------------------------------------------- Style -+" << endl;
	Serial << "| Depth: " << (isHighlighted() ? "foreground" : "background") << endl;
	Serial << "| " << (isAnimated() ? "Is" : "Not") << " animated" << endl;
	Serial << "| Starts on frame " << (startsOnFrameTwo() ? "2" : "1") << endl;
	Serial << "+---------------------------------------- Collision Flags -+" << endl;
	Serial << (isSolid() ? "| Is" : "| Not") << " solid" << endl;
	if (touchTriggersFlag()) {
		if (touchFlipsFlag()) {
			Serial << "| Flips flag " << touchTriggersFlagId() << endl;
		} else {
			Serial << "| Sets flag " << touchTriggersFlagId() << " to "
					 << (touchSetsFlagTo() ? "true" : "false") << endl;
		}
	} else {
		Serial << "| No effect on any flag" << endl;
	}
	Serial << "+------------------------------------- Collision Teleport -+" << endl;
	if (touchTeleportsToRoom()) {
		if (touchTeleportRequiresFlag()) {
			Serial << "| Only tps player to room " << touchTeleportsToRoomId() << " at xy: ("
					 << touchTeleportsToRoomX() << ", " << touchTeleportsToRoomY() << ") when flag "
					 << touchTeleportRequiresFlagId() << " is set to "
					 << (touchTeleportRequiresFlagSetTo() ? "true" : "false") << endl;
		} else {
			Serial << "| Always tps player to room " << touchTeleportsToRoomId() << " at xy: ("
					 << touchTeleportsToRoomX() << ", " << touchTeleportsToRoomY() << ")" << endl;
		}
	} else {
		Serial << "| Does not tp player to another room" << endl;
	}

	Serial << "+--------------------------------------- Collision Dialog -+" << endl;
	if (touchShowsDialog()) {
		if (dialogRequiresFlag()) {
			Serial << "| Only shows dialog " << touchDialogId() << " when flag "
					 << dialogRequiresFlagId() << " is set to "
					 << (dialogRequiresFlagSetTo() ? "true" : "false") << endl;
		} else {
			Serial << "| Always shows dialog " << touchDialogId() << endl;
		}
	} else {
		Serial << "| Does not show dialog" << endl;
	}

	Serial << "+------------------------------------------------- Update -+" << endl;
	Serial << "| When flag " << updateOnFlagId() << " is set to "
			 << (updateOnFlagSetTo() ? "true" : "false") << endl;
	if (updateAffectsSolidity()) {
		Serial << "| -> set solidity to " << (updateSetsSolidityTo() ? "solid" : "not solid") << endl;
	} else {
		Serial << "| -> ignore solidity" << endl;
	}

	if (updateAffectsFrame()) {
		Serial << "| -> set frame to " << (updateSetsFrameTo() == first_frame ? "1" : "2") << endl;
	} else {
		Serial << "| -> ignore frame" << endl;
	}

	if (updateReplacesWithGameObject()) {
		Serial << "| -> displace with object id: " << updateReplacesWithGameObjectId() << endl;
	} else {
		Serial << "| -> ignore displacement" << endl;
	}
	Serial << "+----------------------------------------------------------+" << endl << endl;
#endif
#endif
}

Coordinates GameObject::getTeleportLocation() {
	Coordinates c;
	c.x = touchTeleportsToRoomX();
	c.y = touchTeleportsToRoomY();
	c.roomId = touchTeleportsToRoomId();
	DEBUG("getTeleportLocation");
	return c;
}

void GameObject::reset() {
	data.frame1 = 0;
	data.frame2 = 0;
	for (uint8_t i = 0; i < logicDataStructSize; i++) {
		data.logic[i] = 0;
	}
}
