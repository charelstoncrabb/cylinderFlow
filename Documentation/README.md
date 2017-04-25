# cylinderFlow
C++ Finite Element CFD for Planar Flow

Architechture:
> Classes:
 - Options: handles input/command line options for meshing
 - Node: graph node object for use in meshing
 - Facet: class the represents the triangles produced in the mesh
 - Mesh: class that parses input node data and produces Delaunay Traingulation of nodes using divide and conquer algorithm
 - Matrix: class that has overloaded std::vector<double> to allow for matrix operations, including but not limited to arithmetic, computing characteristic polynomials, and a QR eigenvalue solver/singular values solver
 - TODO: Solver: class that uses meshed grid to solve equations (want to keep this as general as possible, to hopefully be able to use the software for many different models)

> Python pre/post-processing:
 - MeshPlot.py: creates 2D plot of output graph written by Mesh() class
 - squareGrid.py: creates a square grid input file of specified size
 - randGrid.py: creates a randomized grid of specified size

> Using:
 - Example input files are included for format demonstration
 - Both code and processors support '-h' flag for syntax help

> Planned Features:
 - Input-specified grid constraints (e.g., flow around a cylinder)
 - Graphical grid input file generator, with possibly incorporating the actual mesher into the GUI