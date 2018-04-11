/*
	Test.exe used to perform calculation on input data.
	With test_op.py used to check the correctness of BigInteger's
	calculation.

	I-O format ( every input/output ending by newline ):

	Input:  First number
	Input:  Second number
	Input:  Operation ( +/-/ * )
	Output: Result
*/


#include <iostream>
#include <string>

#include "../BigInteger.h"


#define PRINT_OP(oper)\
std::cout << ((first) oper ( second)).ToString() << '\n'  



int main()
{
	while (std::cin)
	{
		std::string op;
		std::getline(std::cin, op);
		BigInteger first{ op.c_str() };

		

		std::getline(std::cin, op);
		BigInteger second{ op.c_str() };

		std::getline(std::cin, op);
		switch (op[0])
		{
		case '+':
			PRINT_OP(+);
			break;

		case '-':
			PRINT_OP(-);
			break;

		case '*':
			PRINT_OP(*);
			break;

		default:
			break;
		}
	}
}