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
    std::string infile(void) const{return inFile;};
    std::string outfile(void) const{return outFile;};
    bool plot(void){return plotFlag;};
    bool rotflag(void){return rotFlag;};
    bool run;
private:
    void printOptions(void);
    int parseBoolFlag(const char* flag);
    std::string inFile;
    std::string outFile;
    bool plotFlag;
    bool rotFlag;
};

#endif /* Options_hpp */
