# BigInteger #
BigInteger it's a simple implementation of long arithmetic in C++.

All allowed operations represented as overloaded operators.

# Usage #
You can just compile include BigInteger.h to your code, compile BigInteger.cpp and link.

Tested compiler versions: MSVC 19.12.25831 and  gcc 4.6.3

Simple example of using:

      BigInteger First { "1234567890345456786789876543" };
      BigInteger Second { "32454540350898353072283852935" };
      
      First += Second;
      // First now 33689108241243809859073729478
      std::cout << First.ToString() << std::endl;

# Internal representation #

Whole unsigned number consists of chunks, each of them represent 9 decimal digits,
maximal decimal value in single chunk is 999999999,
Less significant chunks located first.

Sign and count of used chunks stored in Rank.
Absolute value of Rank - number of used chunks, sign - sign of number

Example:
Rank = -3
Chunks:

        ------------------------------------
        | 123456789 | 098765432 |	      1 |
        ------------------------------------

Stored number is:
-10987654321123456789


# TODO #
- Division
- Modulus
- Another way to reorganize chunks - use all chunk capacity, it makes possible to implement bitwise operations.
- Reorganize code
- More tests
