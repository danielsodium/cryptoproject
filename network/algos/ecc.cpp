#include "ecc.h"
#include <sstream>
#include <iomanip>

namespace ECC {
    long a = 2;
    long b = 3;
    long p = 17;
    Point generator = {5, 1};

    bool is_on_curve(const Point& point) {
        // infinity
        if (point.x == 0 && point.y == 0) {
            return true;
        }
        
        long left = (point.y * point.y) % p;
        if (left < 0) left += p;
        
        long right = (point.x * point.x * point.x) % p;
        right = (right + a * point.x) % p;
        right = (right + b) % p;
        if (right < 0) right += p;
        
        return left == right;
    }

    Point add(const Point& p1, const Point& p2) {
        // infinity
        if (p1.x == 0 && p1.y == 0) return p2;
        if (p2.x == 0 && p2.y == 0) return p1;
        
        // P + (-P) = infinity
        if (p1.x == p2.x && (p1.y + p2.y) % p == 0) {
            return {0, 0};
        }
        
        long lambda, x3, y3;
        
        if (p1.x == p2.x && p1.y == p2.y) {
            long numerator = (3 * p1.x * p1.x + a) % p;
            long denominator = (2 * p1.y) % p;
            
            long inv = 1;
            for (int i = 1; i < p; i++) {
                if ((denominator * i) % p == 1) {
                    inv = i;
                    break;
                }
            }
            
            lambda = (numerator * inv) % p;
        } else {
            long numerator = (p2.y - p1.y) % p;
            long denominator = (p2.x - p1.x) % p;
            if (numerator < 0) numerator += p;
            if (denominator < 0) denominator += p;
            
            long inv = 1;
            for (int i = 1; i < p; i++) {
                if ((denominator * i) % p == 1) {
                    inv = i;
                    break;
                }
            }
            
            lambda = (numerator * inv) % p;
        }
        
        x3 = (lambda * lambda - p1.x - p2.x) % p;
        if (x3 < 0) x3 += p;
        
        y3 = (lambda * (p1.x - x3) - p1.y) % p;
        if (y3 < 0) y3 += p;
        
        return {x3, y3};
    }

    Point multiply(const Point& point, long scalar) {
        if (scalar == 0) {
            return {0, 0};
        }
        
        if (scalar < 0) {
            Point negated = multiply(point, -scalar);
            negated.y = p - negated.y;
            return negated;
        }
        
        Point result = {0, 0};
        Point temp = point;
        
        while (scalar > 0) {
            if (scalar & 1) {
                result = add(result, temp);
            }
            
            temp = add(temp, temp);
            
            scalar >>= 1;
        }
        
        return result;
    }

    std::string serialize(const Point& point) {
        std::stringstream ss;
        ss << "(" << point.x << "," << point.y << ")";
        return ss.str();
    }

    Point deserialize(const std::string& str) {
        Point result = {0, 0};
        
        size_t comma_pos = str.find(',');
        if (comma_pos != std::string::npos && str.front() == '(' && str.back() == ')') {
            std::string x_str = str.substr(1, comma_pos - 1);
            std::string y_str = str.substr(comma_pos + 1, str.length() - comma_pos - 2);
            
            try {
                result.x = std::stol(x_str);
                result.y = std::stol(y_str);
            } catch (...) {
                result = {0, 0};
            }
        }
        
        return result;
    }
}