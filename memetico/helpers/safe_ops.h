
#ifndef MEMETICO_MODELS_SAFE_OPS_H_
#define MEMETICO_MODELS_SAFE_OPS_H_

#include <stdlib.h>
#include <string>
#include <stdexcept>
#include <math.h>
#include <cstring>

//using namespace std;

/**
 * Safe multiplication of a and b
 * 
 * @param a     first value
 * @param b     second value
 * @return      double a * b
 *              throws invalid_argument in case of overflow
 */
inline double multiply(double a, double b) {
    
    // Note there are precision issues cause the division to be unequal. Can implement a better check
    double x = a * b;
    if (a != 0 && abs(x/a-b) > 1) {
        throw invalid_argument("Mutiplication overflow a:"+to_string(a)+" b:"+to_string(b)+" x:"+to_string(x)+" x/a:"+to_string(x/a));   
    }
    return x;
}

/**
 * Safe divide of a and b
 * 
 * @param a     first value
 * @param b     second value
 * @return      double a / b
 *              throws invalid_argument in case of overflow
 */
inline double divide(double a, double b) {
    
    if(a == 0 && b == 0)
        return 0;

    if(b == 0)
        throw invalid_argument("Divide by 0");

    // If the denominator is tiny we can overflow, so I think we need this..

    double x = a / b;
    if (abs(x*b-a) > 1)
        throw invalid_argument("Divide overflow a:"+to_string(a)+" b:"+to_string(b)+" x:"+to_string(x)+" x*b:"+to_string(x*b));   
        
    return x;
}

/**
 * Safe addition of a and b
 * 
 * @param a     first value
 * @param b     second value
 * @return      double a + b
 *              throws invalid_argument in case of overflow
 */
inline double add(double a, double b) {

    double x = a + b;
    if (a > 0 && b > 0 && x < 0) {
        throw invalid_argument("Addition overflow :"+to_string(a)+" b:"+to_string(b)+" x:"+to_string(x));    
    }
    if (a < 0 && b < 0 && x > 0) {
        throw invalid_argument("Addition underflow :"+to_string(a)+" b:"+to_string(b)+" x:"+to_string(x));  
    }
    return x;
}

/**
 * Safe exponent raised to a
 * 
 * @param a     value
 * @return      double exp(a)
 *              throws invalid_argument in case of overflow
 */
inline double exp(double a) {

    double ret = std::exp(a);

    if (errno) 
        throw invalid_argument("Exponent overflow");   

    return ret;   
}

/**
 * Safe raising of power to base 
 * 
 * @param base      base of operation
 * @param power     power of operation
 * @return          double pow(base, power)
 *                  throws invalid_argument in case of overflow
 */
inline double power(double base, double power) {

    errno = 0;
    double ret = std::pow(base, power);
    if (errno) {
        throw invalid_argument("Powers overflow " + string(strerror(errno)));   
    }

    return ret; 
}

#endif