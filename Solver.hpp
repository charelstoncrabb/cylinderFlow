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


class Solver{
    Solver(const Mesh* mesh);
    
private:
    
    // Nested class handling all functions defined on the given mesh:
    class Function{
    public:
        
    private:
        // (Basis) (El)ements functions class:
        class BasisEl{
            BasisEl(const Node* vtx);
        private:
            const Node* vertex;
            std::vector<Facet*> support;
            std::map<Facet*,std::vector<double> > coeffs;
            double norm;
            friend class Solver;
            friend class Function;
        };
    
        // BasisRep = (Basis) (Rep)resentation of a function = linear combination of basis functions class
        class BasisRep{
        private:
            std::map<Solver::Function::BasisEl*,double> basisWeights;
            friend class Solver;
            friend class Function;
        };
        double integratePlane(std::vector<Node*> vertices, std::vector<double> vals);
        friend class Solver;
    };
    
    
    const Mesh* feMesh;
    std::vector<Function::BasisEl*> basis;
    double dt;
};


#endif /* Solver_hpp */
