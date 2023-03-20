
#ifndef MEMETICO_HELPER_EXCEL_H_
#define MEMETICO_HELPER_EXCEL_H_

#include <string>

/**
 * Return Excel cell name for a given column and row number
 * @param col       Excel column number
 * @param row       Excel row number
 * @return          string Excel name e.g.
 *                  (1,1) = A1, (2,2) = B2, (5,4) = E4 
 */
inline string excel_name(size_t col = 2, size_t row = 2) {

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

#endif