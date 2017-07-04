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
#include "Mesh.hpp"


class SolverBase{
public:
    SolverBase(const Mesh* mesh);
    virtual void integrateInTime(double currentTime, double dt) = 0;
private:

    const Mesh* feMesh;
};

#endif /* Solver_hpp */
