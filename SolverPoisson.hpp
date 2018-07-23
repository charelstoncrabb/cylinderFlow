//
//  SolverPoisson.hpp
//  cylinderFlow
//
//  Created by Charelston Crabb on 7/4/17.
//  Copyright © 2017 NLC. All rights reserved.
//
#ifndef _SOLVERPOISSON_HPP_
#define _SOLVERPOISSON_HPP_

#include "Solver.hpp"
#include "Mesh.hpp"

//! Class for implementing Poisson equation on provided finite element mesh. See SolverBase for boundary conditions specifications.
class SolverPoisson : public SolverBase {
public:
	//! Public Constructor
	/*!
		\param mesh <b>(const Mesh*)</b>
		\return Initialized SolverEuler object
	*/
    SolverPoisson(const FEMesh* mesh) : SolverBase(mesh) {};

	//! Time-step the solution
	/*!
	\todo This interface is deprecated, and should be implemented in inherited classes (Poisson solver is not time-dependent).
	*/
	virtual void integrateInTime(void) {};

	//! Compute the current state of the solution
	/*!
	This function computes the current state of the solution
		\param None
		\return None
	*/
	virtual void computeState(void) {};
};


#endif
