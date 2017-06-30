//
//  Math.hpp
//  cylinderFlow
//
//  Created by Nicholas Crabb on 3/26/17.
//  Copyright © 2017 NLC. All rights reserved.
//
#ifndef Math_h
#define Math_h
#include "Includes.hpp"
#include "Mesh.hpp"

namespace Math{
    
    class Combinatorics{
    public:
        Combinatorics(){};
        std::vector<std::vector<int> > nChoosek(std::vector<int> values, int K);
        void testnChoosek(void);
    private:
    };

    class Function{
    public:
    
    private:
        double integratePlane(std::vector<Node*> vertices, std::vector<double> vals);
        friend class Solver;
    };

    class BasisEl : public Function{
        BasisEl(const Node* vtx);
    private:
        const Node* vertex;
        std::vector<Facet*> support;
        std::map<Facet*,std::vector<double> > coeffs;
        double norm;
        friend class Function;
    };

    class BasisRep : public Function{
        BasisRep(){};
    private:
        std::map<BasisEl*,double> basisWeights;
        friend class Solver;
        friend class Function;
    };
}

#endif /* Math_h */
