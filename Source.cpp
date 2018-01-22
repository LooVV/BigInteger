#include <iostream>
#include "BigInteger.h"

int main()
{
	BigInteger big("3213187657342783532747236272272312345");
	BigInteger big2("3213187657342783532747236272272312345");
	BigInteger big3 = big * big2;

	std::cout << (big3).ToString() << '\n';
	big3++;
	std::cout << (big3).ToString() << '\n';
}
