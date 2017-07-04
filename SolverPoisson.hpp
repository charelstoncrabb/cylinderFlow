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

class SolverPoisson : public SolverBase {
    SolverPoisson(const Mesh* mesh) : SolverBase(mesh) {};
};


#endif
