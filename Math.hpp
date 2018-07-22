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

/*!
This namespace contains some helpful mathematical objects and operations
*/
namespace Math{
    
	//! Combinatorics class performs basic combinatorial operations
	/*!
	This class is used for solving basic combinatorial problems.  Currently only "n choose k" is implemented.
	*/
    class Combinatorics{
    public:
		//! Public Constructor
		/*!
			\param None
			\return Initialized Combinatorics object
		*/
        Combinatorics(){};

		//! Solves N choose K problem
		/*!
		This operation takes a vector of integers and a value K, between 0 and N = values.size(), and returns all K-combinations of entries in values
			\param values <B>(std::vector<int>)</B> vector of integer values to be sampled from
			\param K <B>(int)</B> number of samples to select from values
			\return <B>(std::vector<std::vector<int>>)</B> All unique length K selections from argument values.
		*/
        std::vector<std::vector<int> > nChoosek(std::vector<int> values, int K);

		//! Performs basic unit tests on the nChoosek method
		/*!
		This helper method performs a basic nChoosek operation and prints the results to stdout for visual verification.
			\todo Implement automatic testing of the nChoosek operation
		*/
        void testnChoosek(void);
    private:
    };

	//! This class associates a list of
	/*!
	This class implements a 2D, scalar-valued, piece-wise linear function as values associated with each Node object in a given grid.  The API also provides some helpful operations to be performed on the function object.
	*/
    class Function{
    public:
		//! Public Constructor
		/*!
		\todo Implement a constructor with the specified parameters
			\param nodeList <B>(std::vector<Node*>)</B> List of 2D grid points (as Node objects) on which the function object takes values
			\param values <B>(std::vector<double>)</B> List of values function initially takes on each grid point.  It is assumed that this list is the same size as the Node list provided in first argument; optionally, if no values list is provided, all values will be initialized to zero.
			\return Initialized Function object
		*/
		Function() {};

		//! Perform integration of the function over its entire domain
		/*!
		This method will return the definite integral of the function over the entirety of its domain.
			\todo Method is currently not implemented
			\param None
			\return <B>(double)</B> value of the definite integral of the function
		*/
		double integrateOnDomain(void);
    private:
        
        friend class Solver;
    };

	//! Basis element class
	/*!
	Basis element class is the building block of the finite element basis decomposition of a given function.
	*/
    class BasisEl : public Function{
		//! Public Constructor
		/*!
			\param vtx <B>(const Node*)</B> the Node in the Mesh object on which this basis function takes the value 1
			\return Initialized BasisEl object
		*/
        BasisEl(const Node* vtx);
    private:
        const Node* vertex;
        std::vector<Facet*> support;
        std::map<Facet*,std::vector<double> > coeffs;
        double norm;
    };

	//! Basis element representation of a function
	/*!
	This class stores a basis elemenet representation of a function.
	\todo Re-evaluate appropriateness of this class' inheritance from Function class; it seems inappropriate. Intuitively, a basis element representation/decomposition should really just be a list of BasisEl objects along with corresponding scalar coefficients.
	*/
    class BasisRep : public Function{
		//! Public Constructor
		/*!
			\param None
			\return Initialized BasisRep object
		*/
        BasisRep(){};
    private:
        std::map<BasisEl*,double> basisWeights;
        friend class Solver;
    };
}

#endif /* Math_h */
