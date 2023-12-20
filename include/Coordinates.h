#ifndef Coordinates_H
#define Coordinates_H

#include <Arduino.h>

class Coordinates {
	public:
		uint8_t x{}, y{}, roomId{};
		Coordinates(uint8_t x, uint8_t y, uint8_t roomId) {
			this->x = x;
			this->y = y;
			this->roomId = roomId;
		}

		Coordinates() = default;

		boolean xyEqual(Coordinates c) const {
			return (x == c.x and y == c.y);
		}

		boolean fullEqual(Coordinates c) const {
			return (x == c.x and y == c.y and roomId == c.roomId);
		}

		Coordinates& operator=(const Coordinates& c) {
			x = c.x;
			y = c.y;
			roomId = c.roomId;
			return *this;
		}
};

#endif