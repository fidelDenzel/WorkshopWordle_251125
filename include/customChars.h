// CustomChars.h
#pragma once // Prevents the file from being included multiple times
#include <stdint.h> 
#include <Arduino.h>

// Declaration of the constant byte arrays
// The 'extern' keyword tells the compiler the definition is in a different file.
// extern const uint8_t FLIPPED_H[8];
// extern const uint8_t FLIPPED_E[8]; 
// extern const uint8_t DEGREE_SYMBOL[8];
extern byte flippedOne[8];
extern byte blankChar[8];
extern void diagonalFlipChar(byte (&flip_arr)[8], bool type);
extern byte flippedHeitch[8];
extern byte flippedEe[8];
extern byte flippedeL[8];
extern byte flippedOw[8];
extern byte flippedExclamation[8];
// Declare all your custom characters here