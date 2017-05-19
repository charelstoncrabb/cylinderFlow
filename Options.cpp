//
//  Options.cpp
//  cylinderFlow
//
//  Created by Nicholas Crabb on 4/21/17.
//  Copyright © 2017 Nicholas Crabb. All rights reserved.
//

#include "Options.hpp"

Options::Options(int argc, const char* argv[]):plotFlag(false){
    run = false;
    int pf;
    std::vector<const char*> args(argv + 1, argv + argc);
    std::map<std::string,bool> options;
    options["infile"] = false;
    options["outfile"] = false;
    options["plotflag"] = false;
    try{
        if( argc == 2 && !strcmp(args[0],"-h") ){
            printOptions();
        }else if(argc <= 2 || !argc%2 ){
            throw "bad syntax or missing flag definition -- use \'-h\' for options syntax.";
        }else{
            int i = 0;
            while( i < argc-1 ){
                if( strcmp(&args[i][0],"-") && strlen(args[i]) == 2 ){
                    switch(args[i][1]){
                        case 'i':
                            inFile = args[i+1];
                            options["infile"] = true;
                            run = true;
                            break;
                        case 'o':
                            outFile = args[i+1];
                            options["outfile"] = true;
                            break;
                        case 'p':
                            pf = parsePlotFlag(args[i+1]);
                            if( pf == 0 || pf == 1 ){
                                plotFlag = (bool)pf;
                                options["plotflag"] = true;
                            }
                            else
                                throw "bad plot flag -- use \'-h\' for options syntax.";
                            break;
                        default: throw "bad option -- use \'-h\' for options syntax.";
                            break;
                    }
                }else{
                    throw "bad syntax -- use \'-h\' for options syntax.";
                }
                i += 2;
            }
            if( !options["infile"] )
                throw "no input file specified.";
            if( !options["outfile"] )
                outFile = "default.out";
            if( !options["plotflag"] )
                plotFlag = true;
        }
    }catch(const char* e){
        std::cout << "ERROR in Options(): " << e << '\n';
        throw std::exception();
    }
}

Options::~Options(){
    
}

void Options::printOptions(){
    std::cout << "\t-i <file>\tspecify input node data with file (required to run program)\n\t-o <file>\tspecify mesh output data file\n\t-p <on/off>\tplot resulting triangulated graph\n";
}

int Options::parsePlotFlag(const char* flag){
    if( !strcmp(flag,"on") )
        return 1;
    else if( !strcmp(flag,"off") )
        return 0;
    else
        return -1;
}