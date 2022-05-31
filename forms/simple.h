
/**
 * @file
 * @author Andrew Ciezak <andy@ium.solutions>
 * @version 1.0
 * @brief Header for simple term terms (e.g. double and int)
 * 
 */

#ifndef FORMS_DOUBLE_H
#define FORMS_DOUBLE_H

using namespace std;

// Local
#include <memetico/globals.h>
#include <memetico/data.h>

// Std Lib
#include <stdlib.h>
#include <typeinfo>
#include <iomanip>
#include <limits>

/**
 * @brief 
 * 
 */
template<class T>
class Simple : public Model<T> {

    protected:

    public:

        // Construction 
        Simple(size_t size = 0) : Model<T>(size) {};

        void    randomise(int min, int max);

        T       get_params(size_t param)    { return this->params[param]; };

};

#include <forms/simple.tpp>

#endif
