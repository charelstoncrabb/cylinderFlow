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
    char plotScript[100];
    strcpy(plotScript, "./ProcScripts/MeshPlot.py ");
    Mesh *myFirstMesh = new Mesh(argv[1]);
    if( argc > 1 ){
        myFirstMesh->writeMesh(argv[2]);
        std::strcat(plotScript, argv[2]);
        std::strcat(plotScript, " &");
        system(plotScript);
    }
    delete myFirstMesh;
    return 1;
}
