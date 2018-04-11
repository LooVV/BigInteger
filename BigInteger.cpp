#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include <stdlib.h>
#include <algorithm>
#include <cmath>
#include "BigInteger.h"


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//					Chunks operations
//		First passed must be mod greater that second
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// Result must be at least MaxSize + 1 size
//first int must be bigger that second modulo
void BigInteger::NumberBase::AddChunks(const uint32_t* int1, const uint32_t* int2, int MaxOperand, int LowerSize, uint32_t* Result)
{
	int Carry = 0;
	size_t i = 0;
	for (; i < LowerSize; ++i)
	{
		uint32_t ChunkRes = int1[i] + int2[i] + Carry;
		Result[i] = ChunkRes % (int)BigInteger::NumberBase::ChunkLimit;
		Carry = ChunkRes / (int)BigInteger::NumberBase::ChunkLimit;
	}
	for (; i < MaxOperand; ++i)
	{
		uint32_t ChunkRes = int1[i] + Carry;
		Result[i] = ChunkRes % (int)BigInteger::NumberBase::ChunkLimit;
		Carry = ChunkRes / (int)BigInteger::NumberBase::ChunkLimit;
	}
	if (Carry)
	{
		Result[MaxOperand] = Carry;
	}
}

void BigInteger::NumberBase::SubtractChunks(const uint32_t* int1, const uint32_t* int2, int MaxOperand, int LowerSize, uint32_t* Result)
{
	unsigned int Take = 0;
	size_t i = 0;
	for (; i < LowerSize; ++i)
	{
		if (int2[i] + Take > int1[i])
		{
			Result[i] = BigInteger::NumberBase::ChunkLimit + int1[i] - int2[i] - Take;
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
			Result[i] = BigInteger::NumberBase::ChunkLimit + int1[i] - Take;
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
void BigInteger::NumberBase::MultiplyChunks(const uint32_t* int1, const uint32_t* int2, int MaxOperand, int LowerSize, uint32_t* Result)
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
			Accumulator = (uint64_t)int1[j] * int2[i] + Carry;
			Carry = Accumulator / BigInteger::NumberBase::ChunkLimit;
			
			TmpMass[j + Offset] = Accumulator % BigInteger::NumberBase::ChunkLimit;
		}
		if (Carry > 0)
			TmpMass[j + Offset] = static_cast<uint32_t>(Carry);
		
		AddChunks(AccMass.data(), TmpMass.data(), static_cast<int>(ResSize), static_cast<int>(ResSize), AccMass.data());
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

BigInteger::BigInteger(const NumberBase& big): _Number{ big }
{
}

BigInteger::BigInteger()
{
}

BigInteger::BigInteger(int Number)
{
	_Number.Chunks.resize(NumberBase::MaxChunksInInteger);
	_Number.Rank  = BigInteger::NumberBase::ConvertInt(Number, _Number.Chunks.data() );
}
BigInteger::BigInteger(unsigned int Number)
{
	_Number.Chunks.resize(NumberBase::MaxChunksInInteger);
	_Number.Rank = BigInteger::NumberBase::ConvertUInt(Number, _Number.Chunks.data());
}
BigInteger::BigInteger(long long Number)
{
	_Number.Chunks.resize(NumberBase::MaxChunksInInteger);
	_Number.Rank = BigInteger::NumberBase::ConvertInt64(Number, _Number.Chunks.data());
}
BigInteger::BigInteger(unsigned long long Number)
{
	_Number.Chunks.resize(NumberBase::MaxChunksInInteger);
	_Number.Rank = BigInteger::NumberBase::ConvertUInt64(Number, _Number.Chunks.data());
}

BigInteger::BigInteger(const char* NumberStr)
{
	if (NumberStr[0] == '-')
	{
		_Number.Rank = -1;
		++NumberStr;
	}
	else
		_Number.Rank = 1;

	size_t StrSize = strlen(NumberStr);

	//check non-digit characters
	for (size_t i = 1; i < StrSize; ++i)	
		if (!isdigit(NumberStr[i]))
		{
			return;
		}

	char Buff[BigInteger::NumberBase::DecimalInChunk + 1] = {};

	int NChunks = (int)std::ceil((double)StrSize / BigInteger::NumberBase::DecimalInChunk);
	_Number.Chunks.resize(NChunks);
	_Number.Rank *= NChunks;
	//begining of the digits for current chunk
	size_t BeginChunkCharacter = 0;

	//number of digits that not fully in BigInteger::NumberBase::DecimalInChunk border
	int HighChunkDigits = StrSize % BigInteger::NumberBase::DecimalInChunk;
	//not fully filled can be only highlest chunk
	if (HighChunkDigits > 0)
	{
		memcpy(Buff, NumberStr, HighChunkDigits);
		_Number.Chunks[NChunks - 1] = strtoul(Buff, NULL, 10);
		BeginChunkCharacter += HighChunkDigits;
		--NChunks;
	}
	//it will decrement in the beginning of the cycle
	size_t CurChunk = NChunks;
	while (CurChunk !=0)
	{
		CurChunk--;
		memcpy(Buff, BeginChunkCharacter + NumberStr, BigInteger::NumberBase::DecimalInChunk);
		_Number.Chunks[CurChunk] = strtoul(Buff, NULL, 10);
		BeginChunkCharacter += BigInteger::NumberBase::DecimalInChunk;
	}
}



//+++++++++++++++++++++++++++++++++++++++++++++++++++++
//				Overloaded operators
//+++++++++++++++++++++++++++++++++++++++++++++++++++++


BigInteger BigInteger::operator-()const 
{
	NumberBase Negative{ _Number };
	Negative.Rank = -Negative.Rank;
	return Negative;
}
BigInteger BigInteger::operator+()const 
{
	return BigInteger(*this);
}


BigInteger BigInteger::operator+( const BigInteger& i2) const 
{
	BigInteger Result;
	NumberBase::_AddInOrder(this->_Number, i2._Number, Result._Number);
	return Result;
}

BigInteger BigInteger::operator-( const BigInteger& i2) const
{
	BigInteger Result;
	NumberBase::_AddInOrder(this->_Number, (-i2)._Number, Result._Number);
	return Result;
}

//dont make new instance instance
BigInteger BigInteger::operator-( BigInteger& i2) const
{
	BigInteger Result;
	//save and invert sign
	int SaveRank = i2._Number.Rank;
	i2._Number.Rank *= -1;

	NumberBase::_AddInOrder(this->_Number, i2._Number, Result._Number);
	i2._Number.Rank = SaveRank;

	return Result;
}

BigInteger BigInteger::operator*( const BigInteger& i2) const
{
	BigInteger Result;
	NumberBase::_MultiplyInOrder(this->_Number, i2._Number, Result._Number);
	return Result;
}



BigInteger& BigInteger::operator+=(  const BigInteger& i2)
{
	NumberBase::_AddInOrder(this->_Number, i2._Number, this->_Number);
	return *this;
}
BigInteger& BigInteger::operator-=( const BigInteger& i2)
{
	NumberBase::_AddInOrder(this->_Number, (-i2)._Number, this->_Number);
	return *this;
}
BigInteger& BigInteger::operator-=( BigInteger& i2)
{
	int SaveRank = i2._Number.Rank;
	i2._Number.Rank *= -1;

	NumberBase::_AddInOrder(this->_Number, i2._Number, this->_Number);
	i2._Number.Rank = SaveRank;
	return *this;
}
BigInteger& BigInteger::operator*=( const BigInteger& i2)
{
	NumberBase::_MultiplyInOrder(this->_Number, (-i2)._Number, this->_Number);
	return *this;
}


//	increment/decrement

BigInteger& BigInteger::operator++()
{
	*this += 1;
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
	*this -= 1;
	return *this;
}

BigInteger BigInteger::operator--(int)
{
	BigInteger Tmp = *this;
	--(*this);
	return Tmp;
}



//+++++++++++++++++++++++++++++++++++++++++++++++++++++
//					Other operators
//+++++++++++++++++++++++++++++++++++++++++++++++++++++

BigInteger BigInteger::Abs() const 
{
	BigInteger Int{ *this };
	Int._Number.Rank = abs(Int._Number.Rank);
	return Int;
}


std::string BigInteger::ToString()
{
	if (_Number.Rank == 0)
		return "0";

	std::string Res;
	if (_Number.Rank < 0)
		Res.push_back('-');

	size_t i = abs(_Number.Rank) - 1;

	//need first without padding
	Res += std::to_string(_Number.Chunks[i]);

	while (i != 0)
	{
		--i;
		std::string Digits = std::to_string(_Number.Chunks[i]);
		Res += std::string(BigInteger::NumberBase::DecimalInChunk - Digits.size(), '0').append(Digits);
	}
	return Res;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++
//					Utility functions
//+++++++++++++++++++++++++++++++++++++++++++++++++++++


bool BigInteger::NumberBase::IsSameSign(int sign1, int sign2)
{
	return sign1 >= 0 && sign2 >= 0 || sign1 < 0 && sign2 < 0;
}

// returns max rank for multiplication result
int BigInteger::NumberBase::MultiplyMaxRankResult(int sign1, int sign2)
{
	if (IsSameSign(sign1, sign2))
		return abs(sign1 + sign2);
	else
		return -(abs(sign1) + abs(sign2));
}

// add or substract second arg to get bigger modulo value
// b must be >= 0
int BigInteger::NumberBase::AddModulo(int A, int B)
{
	if (A >= 0)
		return A + B;
	else
		return A - B;
}


bool BigInteger::NumberBase::GreaterModulo(const NumberBase& num1, const NumberBase& num2)
{
	int AbsRank1 = abs(num1.Rank);
	int AbsRank2 = abs(num2.Rank);

	// TODO: why i rewrited this
	if (AbsRank1 > AbsRank2)
		return true;
	else if (AbsRank1 < AbsRank2)
		return false;

	for (int i = abs(num1.Rank) - 1; i >= 0; --i)
	{
		if (num1.Chunks[i] > num2.Chunks[i])
			return true;
		else if (num1.Chunks[i] < num2.Chunks[i])
			return false;
	}
	return false;
}

void BigInteger::NumberBase::_RecountRank()
{
	int i = abs(Rank) - 1;
	bool NonZero = false;

	for (; i >= 0 ; --i)
	{
		if (Chunks[i] != 0)
			break;
	}
	
	if (i < 0)
	{
		Rank = 0;
		return;
	}

	if (Rank >= 0)
		Rank = i + 1;
	else
		Rank = -(i + 1);
}


//passig mod bigger to functions like _Perform
void BigInteger::NumberBase::_AddInOrder(const NumberBase& num1, const NumberBase& num2, NumberBase& Result)
{
	if (BigInteger::NumberBase::GreaterModulo(num1, num2))
		_PerformAdd(num1, num2, Result);
	else
		_PerformAdd(num2, num1, Result);
}
void BigInteger::NumberBase::_MultiplyInOrder(const NumberBase& num1, const NumberBase& num2, NumberBase& Result)
{
	 if (BigInteger::NumberBase::GreaterModulo(num1, num2))
		 _PerformMultiply(num1, num2, Result);
	 else
		 _PerformMultiply(num2, num1, Result);
}


//perform operation, based on signs of numbers
void BigInteger::NumberBase::_PerformMultiply(const NumberBase& num1, const NumberBase& num2, NumberBase& Result)
{
	Result.Rank = MultiplyMaxRankResult(num1.Rank, num2.Rank);
	Result.Chunks.resize( abs(Result.Rank) );

	MultiplyChunks(num1.Chunks.data(), num2.Chunks.data(), abs(num1.Rank), abs(num2.Rank), Result.Chunks.data());

	Result._RecountRank();
}
void BigInteger::NumberBase::_PerformAdd(const NumberBase& num1, const NumberBase& num2, NumberBase& Result)
{
	Result.Chunks.resize(abs(num1.Rank) + 1);

	Result.Rank = AddModulo(num1.Rank, 1);
	
	if (IsSameSign(num1.Rank, num2.Rank))
	{
		AddChunks(num1.Chunks.data(), num2.Chunks.data(), abs(num1.Rank), abs(num2.Rank), Result.Chunks.data());
	}
	else
	{
		SubtractChunks(num1.Chunks.data(), num2.Chunks.data(), abs(num1.Rank), abs(num2.Rank), Result.Chunks.data());
	}
	Result._RecountRank();
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++
//						Compare
//+++++++++++++++++++++++++++++++++++++++++++++++++++++


bool BigInteger::NumberBase::operator==(const NumberBase& i2) const
{
	if (Rank != i2.Rank)
		return false;

	for (int i = abs(Rank) - 1; i >= 0; --i)
	{
		if (Chunks[i] != i2.Chunks[i])
			return false;
	}
	return true;
}


bool BigInteger::NumberBase::operator>(const NumberBase& i2) const
{
	if (Rank > i2.Rank)
		return true;
	else if (Rank < i2.Rank)
		return false;

	//equal
	for (int i = abs(Rank) - 1; i >= 0; --i)
	{
		if (Chunks[i] > i2.Chunks[i])
			return true;
		else if (Chunks[i] < i2.Chunks[i])
			return false;
	}
	return false;
}
bool BigInteger::NumberBase::operator<(const NumberBase& i2) const
{
	if (Rank < i2.Rank)
		return true;
	else if (Rank > i2.Rank)
		return false;

	//equal
	for (int i = abs(Rank) - 1; i >= 0; --i)
	{
		if (Chunks[i] < i2.Chunks[i])
			return true;
		else if (Chunks[i] > i2.Chunks[i])
			return false;
	}
	return false;
}



bool BigInteger::operator==(const BigInteger& i2) const
{
	return this->_Number == i2._Number;
}
bool BigInteger::operator!=(const BigInteger& i2) const
{
	return !(*this == i2);
}
bool BigInteger::operator>(const BigInteger& i2) const
{
	return this->_Number > i2._Number;
}
bool BigInteger::operator<(const BigInteger& i2) const
{
	return this->_Number < i2._Number;
}
bool BigInteger::operator>=(const BigInteger& i2) const
{
	return *this > i2 || *this == i2;
}
bool BigInteger::operator<=(const BigInteger& i2) const
{
	return *this < i2 || *this == i2;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++
//						Convert
//+++++++++++++++++++++++++++++++++++++++++++++++++++++

int BigInteger::NumberBase::ConvertInt(int number, uint32_t * mas)
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
	mas[0] = number % BigInteger::NumberBase::ChunkLimit;
	mas[1] = (uint32_t)(number / BigInteger::NumberBase::ChunkLimit);
	if (mas[1])
		++Used;

	return Used * Sign;
}
int BigInteger::NumberBase::ConvertUInt(unsigned int number, uint32_t * mas)
{
	if (number == 0)
		return 0;

	int Used = 1;
	mas[0] = number % BigInteger::NumberBase::ChunkLimit;
	mas[1] = (uint32_t)(number / BigInteger::NumberBase::ChunkLimit);
	if (mas[1])
		++Used;

	return Used;
}
int BigInteger::NumberBase::ConvertInt64(long long number, uint32_t * mas)
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
	mas[0] = number % BigInteger::NumberBase::ChunkLimit;
	mas[1] = (uint32_t)((number / BigInteger::NumberBase::ChunkLimit) % BigInteger::NumberBase::ChunkLimit);

	if (mas[1])
		++Used;

	number -= mas[0];
	number -= (long long)mas[1] * BigInteger::NumberBase::ChunkLimit;
	if (number != 0)
	{
		mas[2] =static_cast<uint32_t>( (uint32_t)number / ((long long)ChunkLimit * ChunkLimit));
		++Used;
	}

	return Used * Sign;
}
int BigInteger::NumberBase::ConvertUInt64(unsigned long long number, uint32_t * mas)
{
	if (number == 0)
		return 0;

	int Used = 1;
	mas[0] = number % BigInteger::NumberBase::ChunkLimit;
	mas[1] = (uint32_t)((number / BigInteger::NumberBase::ChunkLimit) % BigInteger::NumberBase::ChunkLimit);

	if (mas[1])
		++Used;

	number -= mas[0];
	number -= (unsigned long long)mas[1] * BigInteger::NumberBase::ChunkLimit;
	if (number != 0)
	{
		mas[2] = static_cast<uint32_t>((uint32_t) number / ((unsigned long long)ChunkLimit * ChunkLimit));
		++Used;
	}

	return Used;
}
