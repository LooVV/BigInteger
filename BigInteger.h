#pragma once
#include <stdint.h>
#include <vector>
#include <string>
class BigInteger
{
public:
	int Sign = 0;
	std::vector<uint32_t> Numbers;
	BigInteger();
	BigInteger(const char* NumberStr);
	BigInteger(int Number);
	BigInteger(unsigned int Number);
	BigInteger(long long Number);
	BigInteger(unsigned long long Number);
	BigInteger(int sign, const std::vector<uint32_t>& bits);

	std::string ToString();
	BigInteger Abs() const ;

	void _RecountChunks();

	static void _PerformAdd(const BigInteger& i1, const BigInteger& i2, BigInteger& Result);
	static void _PerformMultiply(const BigInteger& i1, const BigInteger& i2, BigInteger& Result);

	static void _AddInOrder(const BigInteger& i1, const BigInteger& i2, BigInteger& Result);
	static void _MultiplyInOrder(const BigInteger& i1, const BigInteger& i2, BigInteger& Result);


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
	friend BigInteger operator*(const BigInteger& i1, const BigInteger& i2);

};
