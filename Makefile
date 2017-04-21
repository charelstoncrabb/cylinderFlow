CPP = g++
CFLAGS  = -g -O3 -Wall -std=c++11

default: dtmesh

dtmesh: main.o Matrix.o Math.o Mesh.o VectOps.o
	$(CPP) $(CFLAGS) -o dtmesh main.o Matrix.o Math.o Mesh.o VectOps.o

main.o: main.cpp
	$(CPP) $(CFLAGS) -c main.cpp Matrix.hpp

Matrix.o: Matrix.cpp Matrix.hpp
	$(CPP) $(CFLAGS) -c Matrix.cpp

Math.o: Math.cpp Math.hpp
	$(CPP) $(CFLAGS) -c Math.cpp

Mesh.o: Mesh.cpp Mesh.hpp
	$(CPP) $(CFLAGS) -c Mesh.cpp

VectOps.o: VectOps.cpp VectOps.hpp
	$(CPP) $(CFLAGS) -c VectOps.cpp

clean:
	$(RM) dtmesh *.o *~ *.gch *.out
