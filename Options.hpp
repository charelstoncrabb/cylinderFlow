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

//! Class that handles program command line options
/*!
This class handles the program execution options, parsed from the command line arguments.
*/
class Options{
public:
	//! Public Constructor
	/*!
		\param argc <b>(int)</b> argc value from main() function; used for parsing command line arguments.
		\param argv <b>(const char **argv)</b> argv variable from main() function; used for parsing command line arguments.
		\return Initialized Options object
	*/
    Options(int argc, const char* argv[]);

	//! Destructor
    ~Options();

	//! Command line options input filename access
	/*!
	This function provides public access to the command line arg-provided <em>input</em> Mesh data filename
		\param None
		\return <b>(std::string)</b> Mesh input data filename, as parsed from command line arguments
	*/
    std::string infile(void) const{return inFile;};

	//! Command line options output filename access
	/*!
	This function provides public access to the command line arg-provided <em>output</em> Mesh data filename.  This is the name of the file that is written to with the Mesh method Mesh::writeMesh(std::string).
	\param None
	\return <b>(std::string)</b> Mesh output data filename, as parsed from command line arguments
	*/
    std::string outfile(void) const{return outFile;};

	//! Command line options plot mesh flag
	/*!
	This function provides public acccess to the command line arg-provided plot flag. If the option <b>-p</b> is included with the command line arguments, then the program will produce a plot of the computed Mesh after execution.
		\param None
		\return <b>(bool)</b> True if the "plot computed Mesh" flag has been specified in the command line options
	*/
    bool plot(void){return plotFlag;};

	//! Command line options rotate grid flag
	/*!
	This function provides public acccess to the command line arg-provided rotate grid flag. If the option <b>-r</b> is included with the command line arguments, then the program will perform a linear rotation on the grid points before triangulation; the grid is then rotated back to original specifications after triangulation.
		\param None
		\return <b>(bool)</b> True if the "rotate grid points" flag has been specified in the command line options
	*/
    bool rotflag(void){return rotFlag;};

	//! Boolean value that is set to true if a valid Mesh data input file has been provided, or the -h help flag has not been included in command line args. When set to True, the full program will be executed.
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
