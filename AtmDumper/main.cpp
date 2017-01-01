#include <iostream>
#include "AtmDumper.h"

int testlen;
char test[] = "Get Hacked noob";
bool fl = false;

int testlen_offset = 0x00E8B010;
int test_offset = 0x00E8B000;
int Okay_offset = 0x00E8B014;

int main()
{
	AtmDumper::Debuguer debug;
	if (!debug.attach(L"TestApps.exe"))
		return -1;
	testlen = debug.read<int>(testlen_offset);
	std::cin.get();
	debug.write(test_offset,(PBYTE) test, testlen);
	std::cin.get();
	debug.write<bool>(Okay_offset, fl);
	std::cin.get();
	return 0;
}