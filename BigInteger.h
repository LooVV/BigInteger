#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <stdint.h>
#include <vector>
#include <string>



class BigInteger
{
public:
	BigInteger();
	BigInteger(const char* NumberStr);
	BigInteger(int Number);
	BigInteger(unsigned int Number);
	BigInteger(long long Number);
	BigInteger(unsigned long long Number);

	std::string ToString();

	
	BigInteger Abs() const;

	bool operator==(const BigInteger& i2) const;
	bool operator!=(const BigInteger& i2) const;
	bool operator> (const BigInteger& i2) const;
	bool operator< (const BigInteger& i2) const;
	bool operator>=(const BigInteger& i2) const;
	bool operator<=(const BigInteger& i2) const;


	BigInteger operator-() const;
	BigInteger operator+() const;

	BigInteger& operator++();
	BigInteger operator++(int);
	BigInteger& operator--();
	BigInteger operator--(int);

	BigInteger operator+( const BigInteger& i2) const;
	BigInteger operator-( const BigInteger& i2) const;
	BigInteger operator-( BigInteger& i2)		const;
	BigInteger operator*( const BigInteger& i2) const;

	BigInteger& operator+=( const BigInteger& i2);
	BigInteger& operator-=( const BigInteger& i2);
	BigInteger& operator-=( BigInteger& i2);
	BigInteger& operator*=( const BigInteger& i2);

private:
	struct NumberBase
	{
		std::vector<uint32_t> Chunks;
		int Rank = 0;

		static const int MaxChunksInInteger = 3;
		static const int ChunkLimit = 1000000000;
		static const int DecimalInChunk = 9;

		// abs(num1) > abs(num2)
		// To avoid creating new instance of BigInteger
		static bool GreaterModulo(const NumberBase& num1, const NumberBase& num2);

		//Rank may change after arithmetic operation, need recount
		void _RecountRank();

		bool operator==(const NumberBase& i2) const;
		bool operator>(const NumberBase& i2) const;
		bool operator<(const NumberBase& i2) const;


		static int ConvertInt(int number, uint32_t * mas);
		static int ConvertUInt(unsigned int number, uint32_t * mas);
		static int ConvertInt64(long long number, uint32_t * mas);
		static int ConvertUInt64(unsigned long long number, uint32_t * mas);



		static bool IsSameSign(int sign1, int sign2);
		static int MultiplyMaxRankResult(int sign1, int sign2);
		static int AddModulo(int A, int B);


		static void AddChunks(const uint32_t* int1, const uint32_t* int2, int MaxOperand, int LowerSize, uint32_t* Result);
		static void SubtractChunks(const uint32_t* int1, const uint32_t* int2, int MaxOperand, int LowerSize, uint32_t* Result);
		static void MultiplyChunks(const uint32_t* int1, const uint32_t* int2, int MaxOperand, int LowerSize, uint32_t* Result);


		static void _PerformAdd(const NumberBase& num1, const NumberBase& num2, NumberBase& Result);
		static void _PerformMultiply(const NumberBase& num1, const NumberBase& num2, NumberBase& Result);
		static void _AddInOrder(const NumberBase& num1, const NumberBase& num2, NumberBase& Result);
		static void _MultiplyInOrder(const NumberBase& num1, const NumberBase& num2, NumberBase& Result);

	} _Number;

	BigInteger(const NumberBase& big);
};
#endif