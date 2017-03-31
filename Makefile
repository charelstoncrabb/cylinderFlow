CPP = g++
CFLAGS  = -g -Wall -std=c++11

default: mat

mat: main.o Matrix.o Math.o Mesh.o
	$(CPP) $(CFLAGS) -o mat main.o Matrix.o Math.o Mesh.o

main.o: main.cpp
	$(CPP) $(CFLAGS) -c main.cpp Matrix.hpp

Matrix.o: Matrix.cpp Matrix.hpp
	$(CPP) $(CFLAGS) -c Matrix.cpp

Math.o: Math.cpp Math.hpp
	$(CPP) $(CFLAGS) -c Math.cpp

Mesh.o: Mesh.cpp Mesh.hpp
	$(CPP) $(CFLAGS) -c Mesh.cpp

clean:
	$(RM) mat *.o *~ *.gch *.out
