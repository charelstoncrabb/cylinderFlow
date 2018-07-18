//
//  Math.cpp
//  cylinderFlow
//
//  Created by Lauren Guerra on 3/26/17.
//  Copyright © 2017 NLC. All rights reserved.
//

#include "Math.hpp"

// N CHOOSE K RETURNS LIST OF ALL K-FOLD COMBINATIONS OF N INTEGERS IN VALUES (RECURSIVE ALGORITHM)
// Note: If K is >= N, it behaves as though K == N
// PSEUDOCODE:
// Base case: K == N OR K == 1
//   Return {values}
// For i = 0 to K,
//   currentHead[0] <-- values[i]
//   tailCombs = nChoosek(values[i+1:end],K-1)
//   For each tailComb in tailCombs
//     push {currentHead[0],tailComb} to combinations
//   Endfor
// Endfor
// Return combinations
std::vector<std::vector<int>> Math::Combinatorics::nChoosek(std::vector<int> values, int K){
    std::vector<std::vector<int>> combinations, tailCombinations;
    std::vector<int> tail;
    std::vector<int>::iterator tailCombBegin;
    int currentHead, N = (int)values.size();
    if( K >= N ){
        return {values};
    }else if( K == 1 ){
        for(int i = 0; i < N; i++){
            combinations.push_back({values[i]});
        }
        return combinations;
    }else{
        for(int i = 0; i < N-K+1; i++){
            currentHead = values[i];
            tail.clear();
            tailCombinations.clear();
            for( int j = i+1; j < N; j++ ){
                tail.push_back(values[j]);
            }
            tailCombinations = nChoosek(tail,K-1);
            for(size_t l = 0; l < tailCombinations.size(); l++){
                tailCombinations[l].insert(tailCombinations[l].begin(),currentHead);
                combinations.push_back( tailCombinations[l] );
            }
        }
        return combinations;
    }
}

// BASIC TEST FOR nChoosek() FUNCTION
void Math::Combinatorics::testnChoosek(void){
    int k = 4;
    std::vector<int> vals = {1,2,3,4,5};
    std::vector<std::vector<int>> combs = nChoosek(vals,k);
    for(size_t i = 0; i < combs.size(); i++){
        for(size_t j = 0; j < combs[i].size(); j++){
            std::cout << combs[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

// INTEGRATES A PLANAR FUNCTION OVER FACET facet WITH VALUES AT THE VERTICES vals  -------------------
double Math::Function::integratePlane(std::vector<Node*> vertices, std::vector<double> vals){
    return 0.0;
}//---------------------------------------------------------------------------------------------------

// BASIS ELEMENT CONSTRUCTOR  ------------------------------------------------------------------------
Math::BasisEl::BasisEl(const Node* vtx) : vertex(vtx)
{}//---------------------------------------------------------------------------------------------------
