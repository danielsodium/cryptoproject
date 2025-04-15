#ifndef ECC_H 
#define ECC_H 

#include <string>

namespace ECC {
    struct Point {
        long x;
        long y;
    };

    extern Point generator;
    extern long a, b, p;

    Point multiply(const Point& p, long scalar);

    std::string serialize(const Point& p);
    Point deserialize(const std::string& str);

    bool is_on_curve(const Point& p);
}

#endif
