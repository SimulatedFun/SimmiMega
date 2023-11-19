/// Simple bit level operations on variable size data. Can test the bit at some index inside any
/// number to see if it's set or not. Can also set the bit at some index in a number.

#ifndef Bitwise_h
#define Bitwise_h

#include <Arduino.h>

bool testBit(uint64_t data, uint8_t index);
bool testBit(uint8_t data[], uint8_t index);
void setBit(uint8_t* data, uint8_t index, bool value);
void setBit(uint64_t* data, uint8_t index, bool value);
void setBits(uint8_t* data, uint8_t index, uint8_t bit_length, uint8_t value);
uint8_t getBits(uint8_t* data, uint8_t index, uint8_t bit_length);
uint8_t countSetBits(uint64_t number);
void print64Bit(uint64_t number);

#endif
