//
//  FEMesh.hpp
//  cylinderFlow
//
//  Created by Nicholas Crabb on 7/4/17.
//  Copyright © 2017 Nicholas Crabb. All rights reserved.
//

#ifndef FEMesh_hpp
#define FEMesh_hpp

#include "Mesh.hpp"

//! Finite element Mesh class
/*!
This class, inherited from the slightly more general Mesh class, includes the necessary components for running a finite element solver (child of SolverBase).  Specifically, the FEMesh class includes a Mesh object along with the associated element basis function list.
*/
class FEMesh : public Mesh{
public:
	//! Public Constructor
	/*!
	Public constructor for FEMesh performs meshing of given data points, and additionally creates associated finite element basis functions
		\todo Need to implement initialization of BasisEl list associated with mesh
		\param meshDataFilename
		\return Initialized FEMesh object
	*/
    FEMesh(std::string meshDataFilename);

	//! Destructor
	/*!
	Note the destructor is virtual to ensure proper freeing of members if parent class delete is called
	*/
	virtual ~FEMesh() {};

private:

};

#endif
