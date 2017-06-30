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

class SolverEuler : public SolverBase{
public:
    SolverEuler(const Mesh* mesh);
    // Mandatory virtual methods:
    virtual void integrateInTime(double currentTime, double dt);
};

#endif /* SolverEuler_hpp */
