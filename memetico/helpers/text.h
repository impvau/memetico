
#ifndef HELPERS_TEXT_H
#define HELPERS_TEXT_H

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <string>

/**
 * Trim left empty characters of a string
 * 
 * @param str       string to processs
 * @return          string with left spaces removed
 *                  
 */
static inline string ltrim(string s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}
 
/**
 * Trim right empty characters of a string
 * 
 * @param str       string to processs
 * @return          string with right spaces removed
 *                  
 */
static inline string rtrim(string s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

/**
 * Trim left and right empty characters of a string
 * 
 * @param str       string to processs
 * @return          string with right spaces removed
 *                  
 */ 
static inline string trim(string s) {
    return ltrim(rtrim(s));
}

/**
 * Determine true of suffix appears at the end of file
 * @param file string containing a suffix
 * @param suffix substring to check at end of file
 */
static inline bool match_suffix(string file, string suffix) {
    return file.rfind(suffix) == (file.size() - suffix.size());
}

#endif