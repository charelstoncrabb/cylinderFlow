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
    Matrix acceptTest1(0,0,{});
    Combinatorics acceptTest2;
    Mesh myFirstMesh(argv[1]);
    myFirstMesh.writeMesh(argv[2]);
//    acceptTest1.RunMatrixTests();
//    acceptTest2.testnChoosek();
    return 1;
}
