#ifndef ECC_H 
#define ECC_H 

#include <string> 

/**
 * Namespace instead of a class
 * because the graph will be hardcoded
 * 
 * This will have to change drastically from
 * longs to points
 */

namespace ECC {
    class Point {
        long x;
        long y;
    };

    long multiply(long p1, long p2);
    static long generator = 0;

    // coefficients of the equation
    static int degree = 4;
    // static long[] equation = {1,1,1,1,1};

}

#endif
