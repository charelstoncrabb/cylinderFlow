//
//  SolverEuler.hpp
//  cylinderFlow
//
//  Created by Lauren Guerra on 6/23/17.
//  Copyright © 2017 NLC. All rights reserved.
//

#ifndef SolverEuler_hpp
#define SolverEuler_hpp

#include "Includes.hpp"
#include "Solver.hpp"

//! Class for implementing compressible Euler equations on provided finite element mesh. See SolverBase for boundary conditions specifications.
class SolverEuler : public SolverBase{
public:
	//! Public Constructor
	/*!
		\param mesh <b>(const Mesh*)</b>
		\return Initialized SolverEuler object
	*/
    SolverEuler(const Mesh* mesh);
    
	//! Destructor
	/*!
	Note the destructor is virtual to ensure proper freeing of members if parent class delete is called
	*/
	virtual ~SolverEuler() {};

	//! Time-step the solution
	/*!
	\todo This interface is deprecated, and should be implemented within base class' computeState function.
	*/
    virtual void integrateInTime(double currentTime, double dt);

	//! Compute the current state of the solution
	/*!
	This function computes the current state of the solution
		\param None
		\return None
	*/
	virtual void computeState(void) {};
};

#endif /* SolverEuler_hpp */
