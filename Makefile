all: test

CC=g++
CXXFLAGS=-std=c++11 -g
LDFLAGS=-std=c++11 -g

test.o: test.cpp underscore.hpp streams.hpp strings.hpp

test: test.o


clean:
	rm -rf *.o test *~
	
