#include <iostream>


char test[] = "Hello world !!!";
int testlen = sizeof(test) / sizeof(test[0]);
bool okay = true;

int main()
{
	std::cout << "Test offset : " << &test << " - Testlen offset : " << &testlen << " - Okay offset : " << &okay;
	std::cin.get();
	while (okay)
		std::cout << "Test : " << test << std::endl;
	std::cin.get();
}