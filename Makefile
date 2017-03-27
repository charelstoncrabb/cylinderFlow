CPP = g++
CFLAGS  = -g -Wall -std=c++11

default: mat

mat: main.o Matrix.o Math.o
	$(CPP) $(CFLAGS) -o mat main.o Matrix.o Math.o

main.o: main.cpp
	$(CPP) $(CFLAGS) -c main.cpp Matrix.hpp

Matrix.o: Matrix.cpp Matrix.hpp
	$(CPP) $(CFLAGS) -c Matrix.cpp

Math.o: Math.cpp Math.hpp
	$(CPP) $(CFLAGS) -c Math.cpp

clean:
	$(RM) mat *.o *~ *.gch
