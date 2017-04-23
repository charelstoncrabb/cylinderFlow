//
//  Options.hpp
//  cylinderFlow
//
//  Created by Nicholas Crabb on 4/21/17.
//  Copyright © 2017 Nicholas Crabb. All rights reserved.
//
#include "Includes.hpp"
#ifndef Options_hpp
#define Options_hpp

class Options{
public:
    Options(int argc, const char* argv[]);
    ~Options();
    const char* infile(void){return inFile;};
    const char* outfile(void){return outFile;};
    bool plot(void){return plotFlag;};
    bool run;
private:
    void printOptions(void);
    int parsePlotFlag(const char* flag);
    const char* inFile;
    const char* outFile;
    bool plotFlag;
};

#endif /* Options_hpp */
