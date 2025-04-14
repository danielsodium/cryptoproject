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
    long multiply(long p1, long p2);
    static long generator = 0;
}

#endif
