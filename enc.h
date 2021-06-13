#ifndef enc
#define enc
#include <iostream>
#include <string>

using namespace std;

// 8-bit LFSR counter 
void generateIRArray();

// function based on the system type (32/64)-bit
void system_32(string);
void system_64(string);

// Convert the hex value stored in a string into an integer value
void hexToInt_32(string, uint32_t[]);
void hexToInt_64(string, uint64_t[]);

// Convert any number into binary bits and store each bit in the array
void intToBit_32(uint32_t, uint32_t[]);
void intToBit_64(uint64_t, uint64_t[]);

// Encryption algorithem
void katan32_encrypt(const uint32_t[], uint32_t[], const uint32_t[]);
void katan64_encrypt(const uint64_t[], uint64_t[], const uint64_t[]);

#endif
