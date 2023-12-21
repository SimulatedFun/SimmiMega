#ifndef Typedefs_h
#define Typedefs_h

// Compiler stuff:

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;

using str = const String&;
using boo = boolean;
#define def static constexpr

// Game engine related stuff:

using MovementType = uint8_t;
constexpr MovementType movementTarget = 2;
constexpr MovementType movementWander = 1;
constexpr MovementType movementStill = 0;

using frame = boolean;
constexpr frame first_frame = false;
constexpr frame second_frame = true;

struct ScreenPosition {
		uint8_t x : 4;
		uint8_t y : 4;
};

#endif