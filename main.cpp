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
#include "Options.hpp"

int main(int argc, const char * argv[]) {

//    Matrix A(2,2,{0,-1,-1,3});
//    std::vector<double> b = {-0.5,6},
//                        x = A.solveAxb(b);
//    std::cout << x[0] << "," << x[1] << std::endl;
    char meshplot[100] = "./ProcScripts/MeshPlot.py ", amp[3] = " &";
    Options* options = new Options(argc,argv);
    Mesh* mesh = new Mesh(options->infile(),options->rotflag());
    mesh->writeMesh(options->outfile());
    if( options->plot() ){
        std::strcat( std::strcat(meshplot,options->outfile()), amp);
        system( meshplot );
    }
    delete mesh;
    delete options;
    return 1;
}
