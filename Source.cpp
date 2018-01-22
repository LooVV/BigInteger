#include <iostream>
#include "BigInteger.h"

int main()
{
	//BigInteger big = BigInteger{ "6549474476878645465468487" } -0 + 0 - BigInteger{ "5444655465768765457654" } +BigInteger{ "54678786435486463121121212" } ;// -BigInteger{ "35436125151245214" };
	BigInteger big("3213187657342783532747236272272312345");
	BigInteger big2("3213187657342783532747236272272312345");
	//BigInteger big("321318");
	//BigInteger big2("3213187");
	BigInteger big3 = big * big2;

	std::cout << (big3).ToString();
}
