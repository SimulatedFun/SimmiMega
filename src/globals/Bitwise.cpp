#include "globals/Bitwise.h"

bool Bits::testBit(uint64_t data, uint8_t index) {
	return ((data >> index) & 1) != 0;
}

bool Bits::testBit(const uint8_t data[], uint8_t index) {
	// Calculate the byte index and bit index within that byte
	const uint8_t byteIndex = index / 8;
	const uint8_t bitIndex = index % 8;

	// Use bitwise operators to test the bit at the specified index
	return (data[byteIndex] & (1 << bitIndex)) != 0;
}

void Bits::setBit(uint64_t* data, uint8_t index, bool value) {
	// Create a mask with a 1 at the specified index
	const uint64_t mask = static_cast<uint64_t>(1) << index;

	if (value) {
		// Set the bit at the index to 1
		*data |= mask;
	} else {
		// Set the bit at the index to 0
		*data &= ~mask;
	}
}

void Bits::setBit(uint8_t* data, uint8_t index, bool value) {
	// Calculate the byte index and bit index within that byte
	const uint8_t byteIndex = index / 8;
	const uint8_t bitIndex = index % 8;

	// Create a mask with a 1 at the specified bit index
	const uint8_t mask = static_cast<uint8_t>(1) << bitIndex;

	if (value) {
		// Set the bit at the index to 1
		data[byteIndex] |= mask;
	} else {
		// Set the bit at the index to 0
		data[byteIndex] &= ~mask;
	}
}

void Bits::setBits(uint8_t* data, uint8_t index, uint8_t bit_length, uint8_t value) {
	// Calculate the byte index for the starting bit
	uint8_t byteIndex = index / 8;

	// Calculate the bit offset within the first byte
	uint8_t bitOffset = index % 8;

	// Calculate the number of bits that can be written in the first byte
	uint8_t bitsInFirstByte = 8 - bitOffset;

	if (bit_length <= bitsInFirstByte) {
		// All bits fit within the first byte
		const uint8_t mask = (1 << bit_length) - 1;
		data[byteIndex] &= ~(mask << bitOffset);  // Clear bits to be written
		data[byteIndex] |= ((value & mask) << bitOffset);  // Set the bits
	} else {
		// Write bits that span multiple bytes
		uint8_t bitsToWrite = bit_length;
		uint8_t currentBit = 0;

		while (bitsToWrite > 0) {
			const uint8_t bitsInCurrentByte = (bitsToWrite <= bitsInFirstByte) ? bitsToWrite : bitsInFirstByte;
			const uint8_t mask = (1 << bitsInCurrentByte) - 1;

			data[byteIndex] &= ~(mask << bitOffset);  // Clear bits to be written

			const uint8_t bitsToShift = value >> currentBit;
			data[byteIndex] |= ((bitsToShift & mask) << bitOffset);  // Set the bits

			// Update indices and counts for the next iteration
			byteIndex++;
			bitOffset = 0;
			bitsInFirstByte = 8;
			bitsToWrite -= bitsInCurrentByte;
			currentBit += bitsInCurrentByte;
		}
	}
}

uint8_t Bits::getBits(const uint8_t* data, uint8_t index, uint8_t bit_length) {
	// Calculate the byte index for the starting bit
	uint8_t byteIndex = index / 8;

	// Calculate the bit offset within the first byte
	uint8_t bitOffset = index % 8;

	// Initialize the result
	uint8_t result = 0;

	// Read bits that span multiple bytes
	uint8_t bitsToRead = bit_length;
	uint8_t currentBit = 0;

	while (bitsToRead > 0) {
		const uint8_t bitsInCurrentByte = (bitsToRead <= 8 - bitOffset) ? bitsToRead : (8 - bitOffset);
		const uint8_t mask = (1 << bitsInCurrentByte) - 1;

		const uint8_t bitsRead = (data[byteIndex] >> bitOffset) & mask;
		result |= (bitsRead << currentBit);

		// Update indices and counts for the next iteration
		byteIndex++;
		bitOffset = 0;
		bitsToRead -= bitsInCurrentByte;
		currentBit += bitsInCurrentByte;
	}

	return result;
}

uint8_t Bits::countSetBits(uint64_t n) {
	int count = 0;
	while (n != 0) {
		n &= (n - 1);
		count++;
	}
	return count;
}

void Bits::print64Bit(uint64_t number) {
	uint8_t buf[64];
	uint8_t i = 0;

	if (number == 0) {
		Serial.print('0');
		return;
	}
	while (number > 0) {
		const uint64_t q = number / 10;
		buf[i++] = number - q * 10;
		number = q;
	}
	for (; i > 0; i--) {
		Serial.write(static_cast<char>(buf[i - 1] < 10 ? '0' + buf[i - 1] : 'A' + buf[i - 1] - 10));
	}
}