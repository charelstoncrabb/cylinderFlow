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

class Math{
public:
    Math();
    
    const double PI;
};

class Combinatorics : public Math{
public:
    Combinatorics(){};
    std::vector<std::vector<int> > nChoosek(std::vector<int> values, int K);
    void testnChoosek(void);
private:
};

#endif /* Math_h */
