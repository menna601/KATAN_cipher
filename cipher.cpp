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
uint32_t plainText_32[32];
uint32_t cipherText_32[32];
uint32_t keyArray_32[80];
uint32_t counter_32 = 0;

uint64_t IR_64[254];
uint64_t plainText_64[64];
uint64_t cipherText_64[64];
uint64_t keyArray_64[80];
uint64_t counter_64 = 0;


// x_32 & y_32 are specific bits only used for 32-bit enc
const uint32_t x_32[5] = { 12, 7, 8, 5, 3 };
const uint32_t y_32[6] = { 18, 7, 12, 10, 8, 3 };

const uint64_t x_64[5] = { 24, 15, 20, 11, 9 };
const uint64_t y_64[6] = { 38, 25, 33, 21, 14, 9 };

void system_32(string);
void hexToInt_32(string, uint32_t[]);
void intToBit_32(uint32_t, uint32_t[]);
void katan32_encrypt(const uint32_t[], uint32_t[], const uint32_t[]);
void system_64(string );
void hexToInt_64(string, uint64_t[]);
void intToBit_64(uint64_t, uint64_t[]);
void katan64_encrypt(const uint64_t[], uint64_t[], const uint64_t[]);

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

	for (uint64_t i = 0; i < 254; i++)
		IR_64[i] = IR_32[i];
}

void system_32(string key) {

	string plain = "";

	string subPlain;

	// If key is less than 20-hex values ----> the rest will be filled with 0s
	if (key.length() != 20)
		for (uint32_t i = 0; i < plain.length() % 20; i++)
			plain.insert(i, "0");
	//Construct keyArray based on the hex values passed
	hexToInt_32(key, keyArray_32);
	// Reset the counter
	counter_32 = 0;

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
	subPlain = "00000000";

	/*
	* strCounter: helps to determine the start and the end of each block to be encrypted
	* cipherInteger: helps to represent the cipher block as hex value
	* blockCounter: counts the blocks
	*/
	uint32_t strCounter_32 = plain.length() - 1;
	uint32_t cipherInteger_32, blockCounter_32 = 1;

	cout << "BLOCK#\tPLAIN\t\tCIPHER" << endl;
	//Loops over the plain text
	while (strCounter_32 != -1) {
		// Get the block hex value
		for (int i = 7; i >= 0; i--) {
			subPlain[i] = plain[strCounter_32];
			strCounter_32--;
		}

		//Construct plainText block based on the hex values passed
		hexToInt_32(subPlain, plainText_32);
		// Reset the counter
		counter_32 = 0;

		// Start the enc.
		katan32_encrypt(plainText_32, cipherText_32, keyArray_32);

		cipherInteger_32 = 0;
		for (uint32_t i = 0; i < 32; i++) {
			cipherInteger_32 += (1 << i) * cipherText_32[i];
		}

		cout << blockCounter_32 << "\t" << subPlain << "\t" << hex << cipherInteger_32 << endl;
		blockCounter_32++;
	}
}

// Convert the hex value stored in a string into an integer value
void hexToInt_32(string st, uint32_t array[]) {
	uint32_t integer = 0;

	// For loop starting from the end of the string and counting down
	for (uint32_t i = st.length() - 1; i >= 0; i--) {
		if (st[i] >= '0' && st[i] <= '9') {
			integer = st[i] - '0';
			intToBit_32(integer, array);
		}
		else if (st[i] >= 'a' && st[i] <= 'f') {
			integer = st[i] - 'a' + 10;
			intToBit_32(integer, array);
		}
		// Skip the entered spaces
		else if (st[i] == ' ')
			continue;
		// if char is not [0-9] or [a-f] or a space --> error
		else {
			cout << "error: " << st[i] << " in position " << i + 1 << " is incorrect" << endl;
		}

		if (i == 0)
			break;
	}
}

// Convert any number into binary bits and store each bit in the array
void intToBit_32(uint32_t n, uint32_t array[]) {
	for (int i = 0; i < 4; i++) {
		array[counter_32] = n % 2;
		n /= 2;
		counter_32++;
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

/*********************************************************************************************************/
void system_64(string key) {
	string plain = "";
	string subPlain;

	if (key.length() != 20)
		for (uint64_t i = 0; i < (uint64_t)(plain.length() % 20); i++)
		{
			plain.insert(i, "0");
		}
	//Construct keyArray based on the hex values passed
	hexToInt_64(key, keyArray_64);
	// Reset the counter
	counter_64 = 0;

	cout << "***hint*** if the plaintext is not divisible by 16 --> the rest will be filled with 0s ***hint***" << endl;
	cout << "Enter plaintext: ";
	cin >> plain;
	cout << "------------------------------------------------------------------------" << endl;

	// Ensure that the plaintext can be divided into 16-bit blocks
	// if not --> fill the last block with 0s till it's 16 bits

	if (plain.length() % 16 != 0)
		for (uint64_t i = 0; i < plain.length() % 16; i++)
			plain.insert(i, "0");

	//Initialize the block with any values
	subPlain = "0000000000000000";

	/*
	* strCounter: helps to determine the start and the end of each block to be encrypted
	* cipherInteger: helps to represent the cipher block as hex value
	* blockCounter: counts the blocks
	*/
	uint64_t strCounter_64 = plain.length() - 1;
	uint64_t cipherInteger_64, blockCounter_64 = 1;

	cout << "BLOCK#\tPLAIN\t\t\tCIPHER" << endl;
	//Loops over the plain text
	while (strCounter_64 != -1) {
		// Get the block hex value

		for (int i = 15; i >= 0; i--) {
			subPlain[i] = plain[strCounter_64];
			strCounter_64--;
		}

		//Construct plainText block based on the hex values passed
		hexToInt_64(subPlain, plainText_64);

		// Reset the counter
		counter_64 = 0;

		// Start the enc.
		katan64_encrypt(plainText_64, cipherText_64, keyArray_64);

		cipherInteger_64 = 0;
		for (uint64_t i = 0; i < 64; i++) {
			cipherInteger_64 += ((uint64_t)1 << i) * cipherText_64[i];
		}
		cout << blockCounter_64 << "\t" << subPlain << "\t" << hex << cipherInteger_64 << endl;
		blockCounter_64++;
	}
}

// Convert the hex value stored in a string into an integer value
void hexToInt_64(string st, uint64_t array[]) {
	uint64_t integer = 0;

	// For loop starting from the end of the string and counting down
	for (uint64_t i = st.length() - 1; i >= 0; i--) {
		if (st[i] >= '0' && st[i] <= '9') {
			integer = st[i] - (uint64_t)'0';
			intToBit_64(integer, array);
		}
		else if (st[i] >= 'a' && st[i] <= 'f') {
			integer = st[i] - (uint64_t)'a' + 10;
			intToBit_64(integer, array);
		}
		// Skip the entered spaces
		else if (st[i] == ' ')
			continue;
		// if char is not [0-9] or [a-f] or a space --> error
		else {
			cout << "error: " << st[i] << " in position " << i + 1 << " is incorrect" << endl;
		}
		if (i == 0)
			break;

	}
}

// Convert any number into binary bits and store each bit in the array
void intToBit_64(uint64_t n, uint64_t array[]) {
	for (int i = 0; i < 4; i++) {
		array[counter_64] = n % 2;
		n /= 2;
		counter_64++;
	}
}


void katan64_encrypt(const uint64_t plain[64], uint64_t cipher[64], const uint64_t key[80]) {

	uint64_t L1[25], L2[39], k[2 * 254], fa_2, fa_1, fa_0, fb_2, fb_1, fb_0;
	int i, j;

	for (i = 0; i < 39; ++i)
		L2[i] = plain[i];
	for (i = 0; i < 25; ++i)
		L1[i] = plain[i + 39];

	for (i = 0; i < 80; ++i)
		k[i] = key[i];
	for (i = 80; i < 2 * 254; ++i)
		k[i] = k[i - 80] ^ k[i - 61] ^ k[i - 50] ^ k[i - 13];

	for (i = 0; i < 254; ++i) {

		fa_2 = L1[x_64[0]] ^ L1[x_64[1]] ^ (L1[x_64[2]] & L1[x_64[3]]) ^ (L1[x_64[4]] & IR_64[i]) ^ k[2 * i];
		fa_1 = L1[x_64[0] - 1] ^ L1[x_64[1] - 1] ^ (L1[x_64[2] - 1] & L1[x_64[3] - 1]) ^ (L1[x_64[4] - 1] & IR_64[i]) ^ k[2 * i];
		fa_0 = L1[x_64[0] - 2] ^ L1[x_64[1] - 2] ^ (L1[x_64[2] - 2] & L1[x_64[3] - 2]) ^ (L1[x_64[4] - 2] & IR_64[i]) ^ k[2 * i];
		fb_2 = L2[y_64[0]] ^ L2[y_64[1]] ^ (L2[y_64[2]] & L2[y_64[3]]) ^ (L2[y_64[4]] & L2[y_64[5]]) ^ k[2 * i + 1];
		fb_1 = L2[y_64[0] - 1] ^ L2[y_64[1] - 1] ^ (L2[y_64[2] - 1] & L2[y_64[3] - 1]) ^ (L2[y_64[4] - 1] & L2[y_64[5] - 1]) ^ k[2 * i + 1];
		fb_0 = L2[y_64[0] - 2] ^ L2[y_64[1] - 2] ^ (L2[y_64[2] - 2] & L2[y_64[3] - 2]) ^ (L2[y_64[4] - 2] & L2[y_64[5] - 2]) ^ k[2 * i + 1];

		for (j = 24; j > 2; --j)
			L1[j] = L1[j - 3];
		for (j = 38; j > 2; --j)
			L2[j] = L2[j - 3];
		L1[2] = fb_2;
		L1[1] = fb_1;
		L1[0] = fb_0;
		L2[2] = fa_2;
		L2[1] = fa_1;
		L2[0] = fa_0;
	}

	for (i = 0; i < 39; ++i)
		cipher[i] = L2[i];
	for (i = 0; i < 25; ++i)
		cipher[i + 39] = L1[i];

}


int main() {
	string key = "";
	int systemType;


	// Construct the IR array
	generateIRArray();

	cout << "***hint*** if the key is less than 20 hex-values --> the rest will be filled with 0s ***hint***" << endl;
	cout << "Enter the key: ";
	cin >> key;
	cout << endl;

	cout << "Choose if your system is 32-bit or 64-bit:" << endl;
	cout << "1. 32-bit\n2. 64-bit" << endl;
	cout << "user: ";
	cin >> systemType;
	switch (systemType)
	{
	case 1: 
		system_32(key);
		break;

	case 2:
		system_64(key);
		break;

	default:
		break;
	}

}