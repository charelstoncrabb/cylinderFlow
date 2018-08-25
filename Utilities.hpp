//
//  Utilities.hpp
//  cylinderFlow
//
//  Created by Nicholas Crabb on 7/24/2018.
//  Copyright © 2018 Nicholas Crabb. All rights reserved.
//

#ifndef __UTILITIES_H__
#define __UTILITIES_H__

//! Class that tracks number of instatiated objects of type T during program execution
/*!
This templated class uses a global variable to track the number of instantiations of type **T** through incrementing/decrementing the counter variable in the constructor/destructor, respectively. To track number of instances of a class of type **T**, ensure that the class **T** inherits from classInstanceCounter<T>.
*/
template<class T>
class classInstanceCounter {
public:
	//! The constructor increments the global variable ciCounter<T>
	classInstanceCounter() {
		ciCounter++;
	}
	//! The destructor decrements the global variable ciCounter<T>
	~classInstanceCounter() {
		ciCounter--;
	}
protected:
	//!< Data member that tracks the number of instances of type **T**
	/*!
	Note that for any implementation of this class with type **T**, this value should have a final state of **0** on program exit, otherwise a memory leak has ocurred somewhere.
	*/
	static int ciCounter;
};

template<class T>
int classInstanceCounter<T>::ciCounter = 0;

#endif