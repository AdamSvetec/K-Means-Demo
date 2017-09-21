//Adam Svetec
//Cameron Clifton
//CSE 375/475

#include <iostream>
#include <string>

// store all of our command-line configuration parameters

struct config_t {

    int points;

    int clusters;

    int iterations;

    std::string  filename;

    config_t() : points(0), clusters(0), iterations(1), filename("") { }

    void dump();
};
