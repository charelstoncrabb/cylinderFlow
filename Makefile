CPP = g++
CFLAGS  = -g -O3 -Wall -std=c++11

default: cylinderFlow

cylinderFlow: main.o Matrix.o Math.o Mesh.o VectOps.o Options.o
	$(CPP) $(CFLAGS) -o cylinderFlow main.o Matrix.o Math.o Mesh.o VectOps.o Options.o

main.o: main.cpp
	$(CPP) $(CFLAGS) -c main.cpp

Matrix.o: Matrix.cpp Matrix.hpp
	$(CPP) $(CFLAGS) -c Matrix.cpp

Math.o: Math.cpp Math.hpp
	$(CPP) $(CFLAGS) -c Math.cpp

Mesh.o: Mesh.cpp Mesh.hpp
	$(CPP) $(CFLAGS) -c Mesh.cpp

VectOps.o: VectOps.cpp VectOps.hpp
	$(CPP) $(CFLAGS) -c VectOps.cpp

Options.o: Options.cpp Options.hpp
	$(CPP) $(CFLAGS) -c Options.cpp

clean:
	$(RM) cylinderFlow *.o *~ *.gch *.out Documentation/*.log Documentation/*.out Documentation/*.aux Documentation/*.toc Documentation/*.gz
