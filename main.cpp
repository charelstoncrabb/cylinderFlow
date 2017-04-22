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
// TODO: Start writing actual fluid solver
    Options options(argc,argv);
    if(options.run){
        Mesh mesh(options.infile());
        mesh.writeMesh(options.outfile());
        if( options.plot() ){
            char meshplot[27] = "./ProcScripts/MeshPlot.py ", amp[3] = " &";
            system( std::strcat( std::strcat(meshplot,options.outfile()), amp) );
        }
//        delete mesh;
    }
//    delete options;
    return 1;
}
