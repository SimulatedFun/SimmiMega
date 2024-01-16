#ifndef Bitwise_h
#define Bitwise_h

#include <Arduino.h>

/// Simple bit level operations on variable size data. Can exampleCode the bit at some index inside any
/// number to see if it's set or not. Can also set the bit at some index in a number.
namespace Bits {
	/// Test a bit in a byte array at some index
	/// \param data Byte array of data
	/// \param index Index in the data (not the index in the array)
	bool testBit(const uint8_t data[], uint8_t index);

	/// Sets a bit to either 1 or 0 in a byte array
	/// \param data Byte array of data
	/// \param index Index into the data (not the index in the array)
	/// \param value true = 1, false = 0
	void setBit(uint8_t* data, uint8_t index, bool value);

	/// Test a bit in a 64-bit number
	/// \param data 64-bit data
	/// \param index Index in the data (0-63)
	bool testBit(uint64_t data, uint8_t index);

	/// Sets a bit in a 64-bit number to a value
	/// \param data 64-bit pointer to data
	/// \param index Index in the data (0-63)
	/// \param value true = 1, 0 false
	void setBit(uint64_t* data, uint8_t index, bool value);

	/// Gets a set of bits from somewhere inside a larger data value
	/// TODO: this does not work for vales larger than 8 bits! (needs fixing)
	/// \param data 64-bit pointer to data
	/// \param index Index in the data (0-63)
	/// \param bit_length length of data to be returned
	uint8_t getBits(const uint8_t* data, uint8_t index, uint8_t bit_length);

	/// Sets the bits in a larger data number to value
	/// TODO: this does not work for vales larger than 8 bits! (needs fixing)
	/// \param data 64-bit pointer to data
	/// \param index Index in the data (0-63)
	/// \param bit_length length of data to be returned
	/// \param value value to be placed in the larger data value
	void setBits(uint8_t* data, uint8_t index, uint8_t bit_length, uint8_t value);

	/// Returns the number of set bits in a 64-bit number
	/// \param number data to count bits in
	/// \return number of set bits in the number
	uint8_t countSetBits(uint64_t number);

	/// Prints out a 64-bit integer to the serial monitor
	/// \param number the number to print out to serial
	void print64Bit(uint64_t number);
}

#endif
