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

class BasisFcn;

class Solver{
    Solver(Mesh* mesh);
    
private:
    Mesh* feMesh;
    std::vector<BasisFcn*> basis;
    std::vector<BasisFcn*> dBdx;
    std::vector<double> basisWeights;
    
    double dt;
    
    class BasisFcn{
        BasisFcn();
        
    private:
        Node* vertex;
        std::vector<Facet*> support;
        std::vector< std::vector<double> > coeffs;
        double norm;
        
        friend class Solver;
    };
};



#endif /* Solver_hpp */
