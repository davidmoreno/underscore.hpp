all: test

CC=g++
CXXFLAGS=-std=c++11 -g
LDFLAGS=-std=c++11 -g

test.o: test.cpp sequence.hpp generator.hpp string.hpp

test: test.o

gentest.o: gentest.cpp generator.hpp string.hpp

clean:
	rm -rf *.o test *~ gentest
	
