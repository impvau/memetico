
/**
 * @file
 * @author Haoyuan Sun <hsun2@caltech.edu>
 * @author Mohammad Haque <Mohammad.Haque@newcastle.edu.au>
 * @author Andrew Ciezak <andy@ium.solutions>
 * @version 1.0
 * @brief implementation for global variables and functions
 * 
 */

#include <memetico/globals.h>

/**
 * Trim left empty characters of a string
 * 
 * @param str       string to processs
 * @return          string with left spaces removed
 *                  
 */
string  memetico::ltrim(string str) {
    return str;
    //return regex_replace(str, regex("^\\s+"), string(""));
}
 
/**
 * Trim right empty characters of a string
 * 
 * @param str       string to processs
 * @return          string with right spaces removed
 *                  
 */
string  memetico::rtrim(string s) {
    return s;
    //return regex_replace(s, regex("\\s+$"), string(""));
}

/**
 * Trim left and right empty characters of a string
 * 
 * @param str       string to processs
 * @return          string with right spaces removed
 *                  
 */ 
string memetico::trim(string s) {
    return  ltrim( rtrim(s));
}

/**
 * Determine true of suffix appears at the end of file
 * @param file string containing a suffix
 * @param suffix substring to check at end of file
 */
bool memetico::match_suffix(string file, string suffix) {
    return file.rfind(suffix) == (file.size() - suffix.size());
}

/**
 * Return Excel cell name for a given column and row number
 * @param col       Excel column number
 * @param row       Excel row number
 * @return          string Excel name e.g.
 *                  (1,1) = A1, (2,2) = B2, (5,4) = E4 
 */
string memetico::excel_name(size_t col, size_t row) {

    // Determine character given the column
    string str = "";
    while (col > 0) {
        str = (char) (65 + (col - 1) % 26) + str;
        col = (col - 1) / 26;
    }

    // Add row number
    str = str + to_string(row);

    return str;
}

/**
 * Safe multiplication of a and b
 * 
 * @param a     first value
 * @param b     second value
 * @return      double a * b
 *              throws invalid_argument in case of overflow
 */
double memetico::multiply(double a, double b) {
    
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
double memetico::divide(double a, double b) {
    
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
double memetico::add(double a, double b) {

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
double memetico::exp(double a) {

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
double memetico::pow(double base, double power) {

    double ret = std::pow(base, power);
    if (errno) 
        throw invalid_argument("Powers overflow");   

    return ret; 
}
