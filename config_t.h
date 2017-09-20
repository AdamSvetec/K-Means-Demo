// CSE 375/475 Assignment #1
// Fall 2017
//
// Description: This file declares a struct for storing per-execution
// configuration information.

#include <iostream>
#include <string>

// store all of our command-line configuration parameters

struct config_t {

    int points;

    int clusters;

    int iterations;

    std::string  filename;

    // simple constructor
    config_t() : points(0), clusters(0), iterations(1), filename("") { }

    // Print the values of the seed, iters, and name fields
    void dump();
};
