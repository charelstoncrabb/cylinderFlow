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

class Combinatorics{
public:
    Combinatorics(){};
    std::vector<std::vector<int>> nChoosek(std::vector<int> values, int K);
    void testnChoosek(void);
private:
};

class Func2D{
public:
    Func2D();
};
#endif /* Math_h */
