all: test

CC=g++
CXXFLAGS=-std=c++11
LDFLAGS=-std=c++11

test.o: test.cpp underscore.hpp streams.hpp

test: test.o


clean:
	rm -rf *.o test *~
	
