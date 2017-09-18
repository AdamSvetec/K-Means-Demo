//Adam Svetec
//Cameron Clifton
//CSE 375/475

#ifndef POINT_H
#define POINT_H

#include <math.h>

template <typename T>
class point {
    T x;
    T y;

  public:
    point(T x1, T y1): x(x1), y(y1){}

    T distance(point const & p){
        return sqrt( (p.x-x)^2 + (p.y-y)^2); 
    }
};

#endif
