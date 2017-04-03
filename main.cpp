//
//  main.cpp
//  2DNavierStokes
//
//  Created by Nicholas Crabb on 3/23/17.
//  Copyright © 2017 Nicholas Crabb. All rights reserved.
//

#include "Matrix.hpp"
#include "Math.hpp"
#include "Mesh.hpp"


int main(int argc, const char * argv[]) {
// TODO: Add options module (e.g., --help, -i FirstMesh.dat, -o FirstMesh.out, etc...)

    Mesh myFirstMesh(argv[1]);
    myFirstMesh.writeMesh(argv[2]);
    system("./MeshPlot.py");
    return 1;
}
