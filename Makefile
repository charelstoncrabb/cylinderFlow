CPP = g++
CFLAGS  = -g -Wall -std=c++11

default: mat

mat: main.o Matrix.o
	$(CPP) $(CFLAGS) -o mat main.o Matrix.o

main.o: main.cpp
	$(CPP) $(CFLAGS) -c main.cpp Matrix.hpp

Matrix.o: Matrix.cpp Matrix.hpp
	$(CPP) $(CFLAGS) -c Matrix.cpp

clean:
	$(RM) mat *.o *~
