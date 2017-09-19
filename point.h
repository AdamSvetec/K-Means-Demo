//Adam Svetec
//Cameron Clifton
//CSE 375/475

#ifndef POINT_H
#define POINT_H

#include <math.h>
#include <assert.h>

template <typename T>
class point {
    T x;
    T y;

  public:
    point(): x(), y(){}
    point(T x1, T y1): x(x1), y(y1){}

    void operator+=( point<T> const & p ){
        x += p.x;
        y += p.y;
    }

    point<T> operator/( size_t const & s ) const {
        assert(s != 0);
        return point<T>(x/s, y/s);
    }

    T distance(point const & p) const{
        return sqrt( pow((p.x-x),2) + pow((p.y-y),2)); 
    }
};

#endif
