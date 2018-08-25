//
//  Solver.hpp
//  cylinderFlow
//
//  Created by Nicholas Crabb on 4/25/17.
//  Copyright © 2017 Nicholas Crabb. All rights reserved.
//

#ifndef Solver_hpp
#define Solver_hpp
#include "Includes.hpp"
#include "FEMesh.hpp"

//! Abstract base solver class
/*!
This abstract base class provides a minimal public API for computing a finite element solution on the provided FEMesh. The intent here is to provide a flexible architecture to allow for easy future incorporation of different finite element models.
\todo Still need to design & implement boudary condition UI; a need for flexibility in the types of boundary conditions means it should be done within the Solver classes, though there is a strong dependency on the finite element mesh/basis used.
*/
class SolverBase{
public:
	//! Public Constructor
	/*!
		\param mesh <b>(const FEMesh*)</b> The finite element grid and basis functions object on which to apply and perform the particular solution.
		\return Initialized SolverBase object
	*/
    SolverBase(const FEMesh* mesh);

	//! (Pure virtual) Time-step the solution
	/*!
		\todo This interface is deprecated, and should be implemented in inherited classes (not all solvers may be time-dependent).
	*/
    virtual void integrateInTime(void) = 0;

	//! (Pure virtual) Compute the current state of the solution
	/*!
	This function computes the current state of the solution
		\param None
		\return None
	*/
	virtual void computeState(void) = 0;
private:

    const FEMesh* feMesh;

	double currentTime;
	double dt;
};

#endif /* Solver_hpp */
