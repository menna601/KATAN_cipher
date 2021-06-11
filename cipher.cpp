#include <iostream>
#include <string>
using namespace std;
/*
* For any array representing bits --> LSB is stored at index 0
* --Global variables
* IR stands for Irregualer Update
* plainText array: represents the plainText block
* cipherText array: represents the cipherText block
* keyArray: represents the 80-bit key entered by the user
* counter: is used as a counter for different arrays
*/
uint32_t IR_32[254];
uint32_t plainText[32];
uint32_t cipherText[32];
uint32_t keyArray[80];
uint32_t counter = 0;

// x_32 & y_32 are specific bits only used for 32-bit enc
const uint32_t x_32[5] = { 12, 7, 8, 5, 3 };
const uint32_t y_32[6] = { 18, 7, 12, 10, 8, 3 };

// 8-bit LFSR counter 
void generateIRArray() {
	// Initailized with ones
	uint32_t Register[8] = { 1,1,1,1,1,1,1,1 };
	uint32_t function = 0;

	// Loop to generate the 254 bits of IR
	for (uint32_t i = 0; i < 254; i++) {
		// feedback function = x^8 + x^7 + x^5 + x^3 + 1
		function = Register[0] ^ Register[1] ^ Register[3] ^ Register[5];

		// Shift bits by one to the left
		for (uint32_t j = 0; j < 7; j++)
			Register[j] = Register[j + 1];
		//IR bit is the MSB
		IR_32[i] = Register[0];

		// Assign feedback function result to the LSB
		Register[7] = function;
	}
}
// Convert any number into binary bits and store each bit in the array
void intToBit(uint32_t n, uint32_t array[]) {
	for (int i = 0; i < 4; i++) {
		array[counter] = n % 2;
		n /= 2;
		counter++;
	}
}

// Convert the hex value stored in a string into an integer value
void hexToInt(string st, uint32_t array[]) {
	uint32_t integer = 0;

	// For loop starting from the end of the string and counting down
	for (int i = st.length() - 1; i >= 0; i--) {
		if (st[i] >= '0' && st[i] <= '9') {
			integer = st[i] - '0';
			intToBit(integer, array);
		}
		else if (st[i] >= 'a' && st[i] <= 'f') {
			integer = st[i] - 'a' + 10;
			intToBit(integer, array);
		}
		// Skip the entered spaces
		else if (st[i] == ' ')
			continue;
		// if char is not [0-9] or [a-f] or a space --> error
		else {
			cout << "error: " << st[i] << " in position " << i + 1 << " is incorrect" << endl;
		}

	}
}

// Encryption algorithem
void katan32_encrypt(const uint32_t plain[32], uint32_t cipher[32], const uint32_t key[80]) {
	/*
	* L2: is a 19-bit register initialized with plain from bit 0  ---> bit 18
	* L1: is a 13-bit register initialized with plain from bit 19 ---> bit 31
	* fa & fb are functions
	* k is array containing 254 bits = no. of rounds needed
	* i & j  are counters
	*/
	uint32_t L1[13], L2[19], k[2 * 254], fa, fb;
	uint32_t i, j;

	// Initialize L2 register
	for (i = 0; i < 19; ++i)
		L2[i] = plain[i];

	// Initialize L1 register
	for (i = 0; i < 13; ++i)
		L1[i] = plain[i + 19];

	// For the first 80 bits --> k[] = key[]
	for (i = 0; i < 80; ++i)
		k[i] = key[i];
	// The remaining bits are computed by the function
	// k(i) = k(i-80) xor k(i-61) xor k(i-50) xor k(i-13)
	for (i = 80; i < 2 * 254; ++i)
		k[i] = k[i - 80] ^ k[i - 61] ^ k[i - 50] ^ k[i - 13];

	// Iterate 254 rounds
	for (i = 0; i < 254; ++i) {

		// Calculate the fa & fb bits
		fa = L1[x_32[0]] ^ L1[x_32[1]] ^ (L1[x_32[2]] & L1[x_32[3]]) ^ (L1[x_32[4]] & IR_32[i]) ^ k[2 * i];
		fb = L2[y_32[0]] ^ L2[y_32[1]] ^ (L2[y_32[2]] & L2[y_32[3]]) ^ (L2[y_32[4]] & L2[y_32[5]]) ^ k[2 * i + 1];

		//shift the 2 registers by one bit for each
		for (j = 12; j > 0; --j)
			L1[j] = L1[j - 1];
		for (j = 18; j > 0; --j)
			L2[j] = L2[j - 1];

		// Set the bit 0 of L1 & L2 to equal fb & fa respectively 
		L1[0] = fb;
		L2[0] = fa;
	}

	// The cipher block is L2 + L1 respectively
	for (i = 0; i < 19; ++i)
		cipher[i] = L2[i];
	for (i = 0; i < 13; ++i)
		cipher[i + 19] = L1[i];
}

int main() {
	string plain = "";
	string cipher = "";
	string key = "";

	// Construct the IR array
	generateIRArray();

	cout << "***hint*** if the key is less than 20 hex-values --> the rest will be filled with 0s ***hint***" << endl;
	cout << "Enter the key: ";
	cin >> key;
	cout << endl;

	// If key is less than 20-hex values ----> the rest will be filled with 0s
	if (key.length() != 20)
		for (uint32_t i = 0; i < plain.length() % 20; i++)
			plain.insert(i, "0");
	//Construct keyArray based on the hex values passed
	hexToInt(key, keyArray);
	// Reset the counter
	counter = 0;

	cout << "***hint*** if the plaintext is not divisible by 8 --> the rest will be filled with 0s ***hint***" << endl;
	cout << "Enter plaintext: ";
	cin >> plain;
	cout << "------------------------------------------------------------------------" << endl;

	// Ensure that the plaintext can be divided into 8-bit blocks
	// if not --> fill the last block with 0s till it's 8 bits
	if (plain.length() % 8 != 0)
		for (uint32_t i = 0; i < plain.length() % 8; i++)
			plain.insert(i, "0");

	//Initialize the block with any values
	string subPlain = "00000000";
	/*
	* strCounter: helps to determine the start and the end of each block to be encrypted
	* cipherInteger: helps to represent the cipher block as hex value
	* blockCounter: counts the blocks
	*/
	int strCounter = plain.length() - 1;
	int cipherInteger, blockCounter = 1;

	cout << "BLOCK#\tPLAIN\t\tCIPHER" << endl;
	//Loops over the plain text
	while (strCounter != -1) {
		// Get the block hex value
		for (int i = 7; i >= 0; i--) {
			subPlain[i] = plain[strCounter];
			strCounter--;
		}

		//Construct plainText block based on the hex values passed
		hexToInt(subPlain, plainText);

		// Reset the counter
		counter = 0;

		// Start the enc.
		katan32_encrypt(plainText, cipherText, keyArray);

		cipherInteger = 0;
		for (uint32_t i = 0; i < 32; i++) {
			cipherInteger += (1 << i) * cipherText[i];
		}
		cout << blockCounter << "\t" << subPlain << "\t" << hex << cipherInteger << endl;
		blockCounter++;
	}
}