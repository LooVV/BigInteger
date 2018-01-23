/*
	Long Arithmetic 

  Whole unsigned number consists of chunks, that represent 9 decimal digits,
  maximal decimal value in single chunk 999999999,
  Chunks with lover order, are located first in massive

  Sign and count of used chunks stored in Rank
	
  Example:
  Rank = -3
  Chunks:
  ------------------------------------
  | 123456789 | 098765432 |		   1 |
  ------------------------------------

  Stored number is:
	-10987654321123456789
	

  TODO:
- Avoid creation new object with - operations
- Avoid creation new objects when deal with normal numbers
- Avoid vector in _BigNumber
- Rewrite MultiplyChunks without memory allocation
- Add division
- Add other operations (bitwise, itc.)
*/

#pragma once
#include <stdint.h>
#include <vector>
#include <string>

class BigInteger
{
public:
	struct _BigNumber
	{
		std::vector<uint32_t> Chunks;
		int Rank = 0;

		static const int MaxChunksInInteger = 3;
		static const int ChunkLimit = 1000000000;
		static const int DecimalInChunk = 9;
		// Like abs(num1) > abs(num2)
		// To avoid creating new instance of BigInteger
		static bool GreaterModulo(const _BigNumber& num1, const _BigNumber& num2);

		//Rank may change after arithmetic operation, we need recount them
		void _RecountRank();

		//only necessary operators
		bool operator==(const _BigNumber& i2) const;
		bool operator>(const _BigNumber& i2) const;
		bool operator<(const _BigNumber& i2) const;

	} _Number;

	BigInteger();
	BigInteger(const char* NumberStr);
	BigInteger(int Number);
	BigInteger(unsigned int Number);
	BigInteger(long long Number);
	BigInteger(unsigned long long Number);
	BigInteger(const _BigNumber& big);

	std::string ToString();
	BigInteger Abs() const ;

	static void _PerformAdd(const _BigNumber& i1, const _BigNumber& i2, _BigNumber& Result);
	static void _PerformMultiply(const _BigNumber& i1, const _BigNumber& i2, _BigNumber& Result);

	static void inline _AddInOrder(const _BigNumber& i1, const _BigNumber& i2, _BigNumber& Result);
	static void inline _MultiplyInOrder(const _BigNumber& i1, const _BigNumber& i2, _BigNumber& Result);

	static int ConvertInt(int number, uint32_t * mas);
	static int ConvertUInt(unsigned int number, uint32_t * mas);
	static int ConvertInt64(long long number, uint32_t * mas);
	static int ConvertUInt64(unsigned long long number, uint32_t * mas);

	BigInteger operator-() const;
	BigInteger operator+() const;
	
	bool operator==(const BigInteger& i2) const;
	bool operator!=(const BigInteger& i2) const;
	bool operator>(const BigInteger& i2) const;
	bool operator<(const BigInteger& i2) const;
	bool operator>=(const BigInteger& i2) const;
	bool operator<=(const BigInteger& i2) const;

	BigInteger& operator++();
	BigInteger operator++(int);
	BigInteger& operator--();
	BigInteger operator--(int);

	friend BigInteger operator+(const BigInteger& i1, const BigInteger& i2);
	friend BigInteger operator-(const BigInteger& i1, const BigInteger& i2);
	friend BigInteger operator-(const BigInteger& i1, BigInteger& i2);
	friend BigInteger operator*(const BigInteger& i1, const BigInteger& i2);

	friend BigInteger& operator+=(BigInteger& i1, const BigInteger& i2);
	friend BigInteger& operator-=(BigInteger& i1, const BigInteger& i2);
	friend BigInteger& operator-=(BigInteger& i1, BigInteger& i2);
	friend BigInteger& operator*=(BigInteger& i1, const BigInteger& i2);
};
