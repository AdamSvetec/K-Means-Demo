//Adam Svetec
//Cameron Clifton
//CSE 375/475

#include <iostream>
#include "config_t.h"

// Print the values of the points, clusters, iterations, and filename fields as 
//comma separated values, with a header row
void config_t::dump() {
    std::cout << "# points, clusters, iterations, filename" << std::endl;
    std::cout << points << ", "
              << clusters << ", "
              << iterations << ", "
              << filename << std::endl;
}

