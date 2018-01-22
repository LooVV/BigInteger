#pragma once
const int ChunkLimit = 1000000000;
//can be any integer


int ConvertInt(int number, uint32_t * mas)
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
int ConvertUInt(unsigned int number, uint32_t * mas)
{
	if( number )
		return 0;

	int Used = 1;
	mas[0] = number % ChunkLimit;
	mas[1] = (uint32_t)(number / ChunkLimit);
	if (mas[1])
		++Used;

	return Used;
}

int ConvertInt64(long long number, uint32_t * mas)
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
	mas[1] = (uint32_t)(( number / ChunkLimit) % ChunkLimit);
	
	if (mas[1])
		++Used;

	number -= mas[0];
	number -= (long long)mas[1] * ChunkLimit;
	if (number != 0)
	{
		mas[2] = number / ((long long)ChunkLimit * ChunkLimit);
		++Used;
	}

	return Used * Sign;
}

int ConvertUInt64(unsigned long long number, uint32_t * mas)
{
	if(number == 0)
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
		mas[2] = number / ((unsigned long long)ChunkLimit * ChunkLimit);
		++Used;
	}

	return Used;
}
