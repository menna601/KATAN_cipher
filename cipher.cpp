#include <iostream>
#include <string>
#include "enc.h"

using namespace std;

int main() {
	string key = "";
	int systemType;


	// Construct the IR array
	generateIRArray();

	//GUI
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