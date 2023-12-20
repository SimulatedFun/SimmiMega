#ifndef Callback_h
#define Callback_h

#include <Arduino.h>

template <typename T>
class Callback {
	public:
		void bind(void (*inFunction)(T)) {
			function = inFunction;
		}
		void (*function)(T) = nullptr;
};

#endif