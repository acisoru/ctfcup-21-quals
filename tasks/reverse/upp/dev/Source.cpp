#include "Header.h"
#define CONTAINER_SIZE 128 * CRYPTPROTECTMEMORY_BLOCK_SIZE

uint8_t* pEncPassword;

// CUP{8ee5dfeea690b580f46239c4496873c8}
int EncryptPassword(void);
int DecryptPassword(void);
char* Ascii2Hex(uint8_t* inputString, size_t strSize);
std::string Hex2Ascii(std::string);

uint8_t x[16] = { 35, 50, 26, 124, 120, 72, 98, 40, 98, 127, 124, 31, 125, 121, 85, 105 };
uint8_t y[16] = { 113, 57, 39, 31, 45, 54, 91, 82, 29, 118, 114, 47, 116, 21, 16, 111 };

int getXidx(int value)
{
	for (int i = 0; i < 16; i++)
	{
		if (x[i] == value)
			return i;
	}
	return -1;
};

int getYidx(int value)
{
	for (int i = 0; i < 16; i++)
	{
		if (y[i] == value)
			return i;
	}

	return -1;
};

int main()
{
	while (true)
	{
		std::cout << "------- Universal Password Protector -------" << std::endl;
		std::cout << "1. Encrypt password\n" << "2. Decrypt password\n" << "3. Exit\n";
		std::cout << ">>> ";
		int option = 0;
		std::cin >> option;

		if (option < 1 || option > 3)
			return 0;

		switch (option)
		{
			case 1:
				EncryptPassword();
				break;
			case 2:
				DecryptPassword();
				break;
			case 3:
			default:
				return 0;
		}
	}

	return 0;
}

int DecryptPassword(void)
{
	std::string password;
	std::cout << "{?} Enter password(as a hex string): ";
	std::cin >> password;
	
	if (password.size() % 4 != 0)
	{
		std::cout << "{-} Incorrect size!" << std::endl;
		return 1;
	}

	size_t unhexSize = password.size() / 2;
	unsigned char* unhexData = new unsigned char[unhexSize];

	for (int i = 0; i < password.size(); i += 2)
	{
		sscanf_s(password.c_str() + i, "%02x", unhexData + (i/2));
	}

	unsigned char* decryptedText = new unsigned char[unhexSize/2];
	
	for (int i = 0, j = 0; i < unhexSize; i += 2, j++) {
		decryptedText[j] = getXidx(unhexData[i]) * 16 + getYidx(unhexData[i + 1]);
	}

	std::cout << "{!} Decrypted password: " << decryptedText << std::endl;

	delete[] unhexData;
	delete[] decryptedText;
	return 0;
};

int EncryptPassword(void)
{
	std::string password;
	std::cout << "{?} Enter password: ";
	std::cin >> password;

	DWORD cbSize = password.size() * 2;
	//std::cout << "Enc size: " << cbSize << std::endl;
	pEncPassword = new uint8_t[cbSize];
	
	std::memset(pEncPassword, 0, cbSize);

	for (int i = 0, j = 0; i < cbSize; i += 2, j++) {
		pEncPassword[i] = x[password[j] / 16];
		pEncPassword[i + 1] = y[password[j] % 16];
	}

	SecureZeroMemory((PVOID)password.c_str(), password.size());
	std::cout << "{!} Encrypted password: " << Ascii2Hex((uint8_t*)pEncPassword, cbSize) << std::endl;

	return 1;
};

char* Ascii2Hex(uint8_t* inputString, size_t strSize)
{
	char* res = new char[strSize * 2];

	for (int i = 0; i < strSize; i++) {
		sprintf_s(res + (i*2), 4, "%02x", (int)inputString[i]);
	}

	return res;
}