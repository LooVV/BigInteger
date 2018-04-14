CC=g++


CFLAGS=-g -Wall
INCLUDES= -I../


SRCS = Test.cpp

OBJS = $(SRCS:.c=.o)

PROG = test.exe

all:
	$(CC) $(CFLAGS) $(INCLUDES) -o $(PROG) Test.cpp ../BigInteger.cpp

clean:
	rm $(PROG)

