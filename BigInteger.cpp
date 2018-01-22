#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include <stdlib.h>
//without
#include <algorithm>
#include "BigInteger.h"

/*struct NumberContainer
{
	std::vector<uint32_t> Chunks;
	int Rank
};*/

const int ChunkLimit = 1000000000;
const int ChunkMax = 999999999;
const int DecimalInChunk = 9;

static bool isSameSign(int sign1, int sign2)
{
	return sign1 >= 0 && sign2 >= 0 || sign1 < 0 & sign2 < 0;
}

static int MultiplyMaxSignResult(int sign1, int sign2)
{
	if (isSameSign(sign1, sign2))
		return abs(sign1 + sign2);
	else
		return -(abs(sign1) + abs(sign2));
}

static int AddCapacityResult(int sign1, int sign2)
{
	return std::max(abs(sign1), abs(sign2));
}

static int AddModulo(int A, int B)
{
	if (A >= 0)
		return A + B;
	else
		return A - B;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++
//					Chunks operations
//+++++++++++++++++++++++++++++++++++++++++++++++++++++

// Result must be at least MaxSize + 1 size
//first int must be bigger that second module
void AddChunks(const uint32_t* int1, const uint32_t* int2, int MaxOperand, int LowerSize, uint32_t* Result)
{
	int Carry = 0;
	size_t i = 0;
	for (; i < LowerSize; ++i)
	{
		uint32_t ChunkRes = int1[i] + int2[i] + Carry;
		Result[i] = ChunkRes % (int)ChunkLimit;
		Carry = ChunkRes / (int)ChunkLimit;
	}
	for (; i < MaxOperand; ++i)
	{
		uint32_t ChunkRes = int1[i] + Carry;
		Result[i] = ChunkRes % (int)ChunkLimit;
		Carry = ChunkRes / (int)ChunkLimit;
	}
	if (Carry)
	{
		Result[MaxOperand] = Carry;
	}
}

//first int must be bigger that second module
void SubtractChunks(const uint32_t* int1, const uint32_t* int2, int MaxOperand, int LowerSize, uint32_t* Result)
{
	//одинаковые знаки
	int Take = 0;
	size_t i = 0;
	for (; i < LowerSize; ++i)
	{
		if (int2[i] + Take > int1[i])
		{
			Result[i] = ChunkLimit + int1[i] - int2[i] - Take;
			Take = 1;
		}
		else
		{
			Result[i] = int1[i] - int2[i] - Take;
			Take = 0;
		}
	}
	for (; i < MaxOperand; ++i)
	{
		if (Take > int1[i])
		{
			Result[i] = ChunkLimit + int1[i] - Take;
			Take = 1;
		}
		else
		{
			Result[i] = int1[i] - Take;
			Take = 0;
		}
	}
}

//Result must be at least Max * 2  capacity
void MultiplyChunks(const uint32_t* int1, const uint32_t* int2, int MaxOperand, int LowerSize, uint32_t* Result)
{
	std::vector <uint32_t> AccMass;
	std::vector <uint32_t> TmpMass;
	size_t ResSize = LowerSize + MaxOperand;
	AccMass.resize(ResSize + 1);
	TmpMass.resize(ResSize);
	uint64_t Accumulator = 0;
	uint64_t Carry = 0;
	size_t i = 0;
	
	int Offset = 0;
	//lower number
	for (; i < LowerSize; ++i)
	{
		Carry = 0;
		if (Offset != 0)
		{
			memset(TmpMass.data(), 0, sizeof(uint32_t) * (ResSize));
		}//TmpMass[Offset - 1] = 0;
		//clear mas
		int j = 0;
		for (; j < MaxOperand; ++j)
		{
			//TODO: check this
			Accumulator = (uint64_t)int1[j] * int2[i] + Carry;
			Carry = Accumulator / ChunkLimit;
			
			TmpMass[j + Offset] = Accumulator % ChunkLimit;
		}
		if (Carry > 0)
			TmpMass[j + Offset] = Carry;
		
		AddChunks(AccMass.data(), TmpMass.data(), ResSize, ResSize, AccMass.data());
		Offset++;
	}

	for (int i = 0; i < ResSize; ++i)
	{
		Result[i] = AccMass[i];
	}

}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++
//					Constructors
//+++++++++++++++++++++++++++++++++++++++++++++++++++++

BigInteger::BigInteger(int sign, const std::vector<uint32_t>& bits): Numbers{ bits }, Sign{ sign }
{
}

BigInteger::BigInteger()
{
}

BigInteger::BigInteger(int Number)
{
	uint32_t Buff[3] = {};

	Sign = BigInteger::ConvertInt(Number, Buff);
	Numbers = { Buff, Buff + abs(Sign) };
}
BigInteger::BigInteger(unsigned int Number)
{
	uint32_t Buff[3] = {};

	Sign = BigInteger::ConvertUInt(Number, Buff);
	Numbers = { Buff, Buff + abs(Sign) };
}
BigInteger::BigInteger(long long Number)
{
	uint32_t Buff[3] = {};

	Sign = BigInteger::ConvertInt64(Number, Buff);
	Numbers = { Buff, Buff + abs(Sign) };
}
BigInteger::BigInteger(unsigned long long Number)
{
	uint32_t Buff[3] = {};

	Sign = BigInteger::ConvertUInt64(Number, Buff);
	Numbers = { Buff, Buff + abs(Sign) };
}

BigInteger::BigInteger(const char* NumberStr)
{
	if (NumberStr[0] == '-')
	{
		Sign = -1;
		++NumberStr;
	}
	else
		Sign = 1;

	size_t StrSize = strlen(NumberStr);

	//check non-digit characters
	for (size_t i = 1; i < StrSize; ++i)	
		if (!isdigit(NumberStr[i]))
		{
			return;
		}

	char Buff[DecimalInChunk + 1] = {};

	int Chunks = (int)ceil((double)StrSize / DecimalInChunk);
	Numbers.resize(Chunks);
	Sign *= Chunks;
	//begining of the digits for current chunk
	size_t BeginChunkCharacter = 0;

	//number of digits that not fully in DecimalInChunk border
	int HighChunkDigits = StrSize % DecimalInChunk;
	//not fully filled can be only highlest chunk
	if (HighChunkDigits > 0)
	{
		memcpy(Buff, NumberStr, HighChunkDigits);
		Numbers[Chunks - 1] = strtoul(Buff, NULL, 10);
		BeginChunkCharacter += HighChunkDigits;
		--Chunks;
	}
	//it will decrement in the beginning of the cycle
	size_t CurChunk = Chunks;
	while (CurChunk !=0)
	{
		CurChunk--;
		memcpy(Buff, BeginChunkCharacter + NumberStr, DecimalInChunk);
		Numbers[CurChunk] = strtoul(Buff, NULL, 10);
		BeginChunkCharacter += DecimalInChunk;
	}
}


BigInteger BigInteger::Abs() const 
{
	BigInteger Int{ *this };
	Int.Sign = abs(Int.Sign);
	return Int;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++
//				Overloaded operators
//+++++++++++++++++++++++++++++++++++++++++++++++++++++

//unary 
BigInteger BigInteger::operator-()const 
{
	return BigInteger(Sign * -1, Numbers);
}
BigInteger BigInteger::operator+()const 
{
	return BigInteger(Sign, Numbers);
}

//binary 
BigInteger operator+(const BigInteger& i1, const BigInteger& i2)
{
	if (i1.Sign == 0)
		return BigInteger{ i2 };
	else if (i2.Sign == 0)
		return BigInteger{ i1 };

	BigInteger Result;
	BigInteger::_AddInOrder(i1, i2, Result);
	return Result;
}

BigInteger operator-(const BigInteger& i1, const BigInteger& i2)
{
	if (i1.Sign == 0)
		return -i2;
	else if (i2.Sign == 0)
		return BigInteger{ i1 };

	BigInteger Result;
	BigInteger::_AddInOrder(i1, -i2, Result);
	return Result;
}

//TODO: optimize this

BigInteger operator*(const BigInteger& i1, const BigInteger& i2)
{
	if (i1.Sign == 0)
		return i2;
	else if (i2.Sign == 0)
		return i1;

	BigInteger Result;

	BigInteger::_MultiplyInOrder(i1, i2,Result);
	return Result;
}



//increment/decrement

BigInteger& BigInteger::operator++()
{
	*this = (*this) + 1;
	return *this;
}

BigInteger BigInteger::operator++(int)
{
	BigInteger Tmp = *this;
	++(*this);
	return Tmp;
}

BigInteger& BigInteger::operator--()
{
	*this = *this - 1;
	return *this;
}

BigInteger BigInteger::operator--(int)
{
	BigInteger Tmp = *this;
	--(*this);
	return Tmp;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++
//						Compare
//+++++++++++++++++++++++++++++++++++++++++++++++++++++

std::string BigInteger::ToString()
{
	if (Sign == 0 )
		return "0";

	std::string Res;
	if (Sign < 0)
		Res.push_back('-');
	
	size_t i = abs(Sign);
	--i;
	//need first without padding
	Res += std::to_string(Numbers[i]);

	while (i != 0) 
	{
		--i;
		std::string Digits = std::to_string(Numbers[i]);
		Res += std::string(DecimalInChunk - Digits.size(), '0').append(Digits);
	}
	return Res;
} 
void BigInteger::_RecountChunks()
{
	int i = Sign - 1;
	bool NonZero = false;

	for (; i >= 0 ; --i)
	{
		if (Numbers[i] != 0)
			break;
	}
	
	if (i < 0)
	{
		Sign = 0;
		return;
	}

	if (Sign >= 0)
		Sign = i + 1;
	else
		Sign = -(i + 1);
}


void BigInteger::_AddInOrder(const BigInteger& i1, const BigInteger& i2, BigInteger& Result)
{
	//TODO: avoid this
	if (i1.Abs() > i2.Abs())
		_PerformAdd(i1, i2, Result);
	else
		_PerformAdd(i2, i1, Result);
}

 void BigInteger::_MultiplyInOrder(const BigInteger& i1, const BigInteger& i2, BigInteger& Result)
{
	 if (i1.Abs() > i2.Abs())
		 _PerformMultiply(i1, i2, Result);
	 else
		 _PerformMultiply(i2, i1, Result);
}
void BigInteger::_PerformMultiply(const BigInteger& i1, const BigInteger& i2, BigInteger& Result)
{
	Result.Sign = MultiplyMaxSignResult(i1.Sign, i2.Sign);
	Result.Numbers.resize(abs(Result.Sign));

	MultiplyChunks(i1.Numbers.data(), i2.Numbers.data(), abs(i1.Sign), abs(i2.Sign), Result.Numbers.data());

	Result._RecountChunks();
}


//perform operation, based on signs of numbers
void BigInteger::_PerformAdd(const BigInteger& i1, const BigInteger& i2, BigInteger& Result)
{
	Result.Numbers.resize(abs(i1.Sign) + 1);

	if (isSameSign( i1.Sign, i2.Sign))
	{
		AddChunks(i1.Numbers.data(), i2.Numbers.data(), abs(i1.Sign), abs(i2.Sign), Result.Numbers.data());
	}
	else
	{
		Result.Sign = AddModulo( i1.Sign, 1);
		SubtractChunks(i1.Numbers.data(), i2.Numbers.data(), abs(i1.Sign), abs(i2.Sign), Result.Numbers.data());
	}
	Result._RecountChunks();
}
//first awlays > 




//+++++++++++++++++++++++++++++++++++++++++++++++++++++
//						Compare
//+++++++++++++++++++++++++++++++++++++++++++++++++++++
bool BigInteger::operator==(const BigInteger& i2) const
{
	if (Sign != i2.Sign)
		return false;
	
	for (int i = abs(Sign) -1 ; i >= 0; --i)
	{
		if (Numbers[i] != i2.Numbers[i])
			return false;
	}
	return true;
}
bool BigInteger::operator!=(const BigInteger& i2) const
{
	return !(*this == i2);
}

bool BigInteger::operator>(const BigInteger& i2) const
{
	if (Sign > i2.Sign)
		return true;
	else if (Sign < i2.Sign)
		return false;
	
	//equal
	for (int i = abs(Sign) -1 ; i >= 0; --i)
	{
		if (Numbers[i] > i2.Numbers[i])
			return true;
		else if (Numbers[i] < i2.Numbers[i])
			return false;
	}
	return false;
}
bool BigInteger::operator<(const BigInteger& i2) const
{
	if (Sign < i2.Sign)
		return true;
	else if (Sign > i2.Sign)
		return false;

	//equal
	for (int i = abs(Sign) -1 ; i >= 0; --i)
	{
		if (Numbers[i] < i2.Numbers[i])
			return true;
		else if (Numbers[i] > i2.Numbers[i])
			return false;
	}
	return false;
}
bool BigInteger::operator>=(const BigInteger& i2) const
{
	return *this > i2 || *this == i2;
}
bool BigInteger::operator<=(const BigInteger& i2) const
{
	return *this < i2 || *this == i2;
}



int BigInteger::ConvertInt(int number, uint32_t * mas)
{
	int Sign;
	if (number > 0)
		Sign = 1;
	else if (number < 0)
		Sign = -1;
	else
		//number is zero
		return 0;
	//abs
	if (number < 0)
		number = number * -1;

	int Used = 1;
	mas[0] = number % ChunkLimit;
	mas[1] = (uint32_t)(number / ChunkLimit);
	if (mas[1])
		++Used;

	return Used * Sign;
}
int BigInteger::ConvertUInt(unsigned int number, uint32_t * mas)
{
	if (number == 0)
		return 0;

	int Used = 1;
	mas[0] = number % ChunkLimit;
	mas[1] = (uint32_t)(number / ChunkLimit);
	if (mas[1])
		++Used;

	return Used;
}
int BigInteger::ConvertInt64(long long number, uint32_t * mas)
{
	int Sign;
	if (number > 0)
		Sign = 1;
	else if (number < 0)
		Sign = -1;
	else
		//number is zero
		return 0;
	//abs
	if (number < 0)
		number = number * -1;

	int Used = 1;
	mas[0] = number % ChunkLimit;
	mas[1] = (uint32_t)((number / ChunkLimit) % ChunkLimit);

	if (mas[1])
		++Used;

	number -= mas[0];
	number -= (long long)mas[1] * ChunkLimit;
	if (number != 0)
	{
		mas[2] = (uint32_t)number / ((long long)ChunkLimit * ChunkLimit);
		++Used;
	}

	return Used * Sign;
}
int BigInteger::ConvertUInt64(unsigned long long number, uint32_t * mas)
{
	if (number == 0)
		return 0;

	int Used = 1;
	mas[0] = number % ChunkLimit;
	mas[1] = (uint32_t)((number / ChunkLimit) % ChunkLimit);

	if (mas[1])
		++Used;

	number -= mas[0];
	number -= (unsigned long long)mas[1] * ChunkLimit;
	if (number != 0)
	{
		mas[2] = (uint32_t) number / ((unsigned long long)ChunkLimit * ChunkLimit);
		++Used;
	}

	return Used;
}
