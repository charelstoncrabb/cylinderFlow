//
//  Solver.cpp
//  cylinderFlow
//
//  Created by Nicholas Crabb on 4/25/17.
//  Copyright © 2017 Nicholas Crabb. All rights reserved.
//

#include "Solver.hpp"

//----------------------------------------------------------------------------------------------------
//----------------------------------- SOLVER METHODS -------------------------------------------------
//----------------------------------------------------------------------------------------------------

// PUBLIC CONSTRUCTOR  -------------------------------------------------------------------------------
Solver::Solver(const Mesh* mesh) : feMesh(mesh) {
    // Set up basis functions and corresponding gradients on the given mesh
    for( int i = 0; i < feMesh->size(); i++ ){
        Function::BasisEl* newBasisFcn = new Function::BasisEl(feMesh->nodelist(i));
        basis.push_back(newBasisFcn);
    }
    // Set initial and boundary conditions
}//---------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
//------------------------------ SOLVER::FUNCTION METHODS --------------------------------------------
//----------------------------------------------------------------------------------------------------

// INTEGRATES A PLANAR FUNCTION OVER FACET facet WITH VALUES AT THE VERTICES vals  -------------------
double Solver::Function::integratePlane(std::vector<Node*> vertices, std::vector<double> vals){
    return 0.0;
}//---------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
//-------------------------- SOLVER::FUNCTION::BASISEL METHODS ---------------------------------------
//----------------------------------------------------------------------------------------------------


// BASIS ELEMENT CONSTRUCTOR  ------------------------------------------------------------------------
Solver::Function::BasisEl::BasisEl(const Node* vtx) : vertex(vtx){
    support = vertex->isVertexOf;
    double thisNorm = 0.0;
    for(int i = 0; i < support.size(); i++){
        Facet* cf = support[i];
        std::vector<Node*> nodes(3,NULL);
        
        int j = 0;
        for(; cf->nodes[j] != vtx; j++){}
        nodes[0] = cf->nodes[j]; nodes[1] = cf->nodes[(j+1)%3]; nodes[2] = cf->nodes[(j+2)%3];
        
        //compute normal vector
        std::vector<double> normalVect = {nodes[0]->y() - nodes[2]->y(),nodes[0]->x() - nodes[2]->x(), (nodes[1]->x() - nodes[0]->x()) * (nodes[2]->y() - nodes[0]->y()) - (nodes[1]->y() - nodes[0]->y()) * (nodes[2]->x() - nodes[0]->x())};
        
        // currentCoeffs = { a,b,c }, where this basis fcn on this facet(x,y) = a + bx + cy
        std::vector<double> currentCoeffs = {1 + cf->nodes[0]->x() * normalVect[0] / normalVect[2] + cf->nodes[0]->y() * normalVect[1] / normalVect[2], - normalVect[0] / normalVect[2], - normalVect[1] / normalVect[2]};
        coeffs[cf] = currentCoeffs;
        
        // Calculate contribution on this facet to the function's norm
//        thisNorm += Function::integratePlane(nodes,{1.0,0.0,0.0});
    }
}//---------------------------------------------------------------------------------------------------
