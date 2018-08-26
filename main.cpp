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
#include "Includes.hpp"

/*!
Main program execution. Command line options are parsed from argc & argv into the Options class.
*/
int main(int argc, const char * argv[])
{
	if( argc > 1 )
	{
		std::string meshplot = "python ..\\ProcScripts\\MeshPlot.py ", amp = " &";
		Options* options = new Options(argc, argv);

		Mesh* constraintMesh;
		if (options->constraintMeshFile().size())
			constraintMesh = new Mesh(options->constraintMeshFile(), options->rotflag());
		else
			constraintMesh = NULL;

		Mesh* mesh = new Mesh(options->infile(), options->rotflag(), constraintMesh);
		mesh->writeMesh(options->outfile());
		
		if (options->plot()) {
			std::cout << "Plotting constructed mesh..." << std::endl;
			meshplot += options->outfile() + amp;
			system(meshplot.c_str());
		}

		if (constraintMesh != NULL)
			delete constraintMesh;
		delete mesh;
		delete options;
	}

    return 0;
}
