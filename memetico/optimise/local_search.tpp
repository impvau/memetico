
using namespace meme;

/**
 * @file
 * @author andy@impv.au
 * @version 1.0
 * @brief Implementation of local search functions
*/
template <class U>
double local_search::model_evaluate(vector<double>& params, vector<size_t>& positions, U* model, DataSet* data, vector<size_t>& selected) {

    for(size_t i = 0; i < params.size(); i++) {
        model->set_value(positions[i], params[i]);
    }

    return model->objective(data, selected);
}

/**
 * Cusomised Nedler-Mead algorithm
 * 
 * @param model Model to optimise
 * @return double
 * 
 */
template <class U>
double local_search::custom_nelder_mead(U* model, DataSet* data, vector<size_t>& selected) {

    // We consider a three-dimensional problem, there are a series of y(x1,x2) po
    // 
    // - w is the worst y(x1, x2)
    // - b1 is the best y(x1, x2)
    // - b2 is the second best y(x1, x2)
    // - r is a point is the reflected point of the worst through the centroid of b1 and b2
    // - e is the expanded point beyond r
    // - ^ is the contracted point 
    // 
    //          b1    
    //                                  e
    //                       r
    //             c    
    //   w    ^               
    // 
    //                b2

    //local_search::nm_duration = 0;
    //local_search::nm_iters = 0;

    //auto start_nm = chrono::high_resolution_clock::now();

    using coord = vector<double>;

    // Determine the number of parameters to optimise
    size_t params = 0;
    vector<size_t> positions;
    for(size_t i = 0; i < model->get_count(); i++) {
        
        if (model->get_active(i))  {
            params++;
            positions.push_back(i);
        }
    }
        
    // Determine the refleted point
    auto refl = [](const coord& a, const coord & b, size_t dimensions) {

        coord ret(dimensions);
        for (size_t i = 0; i < dimensions; ++i) {
            ret[i] = 2 * a[i] - b[i];
        }
        return ret;
    };

    // Determine the expanded point
    auto expa = [](const coord& a, const coord & b, size_t dimensions) {
        coord ret(dimensions);
        for (size_t i = 0; i < dimensions; ++i) {
            ret[i] = 3 * a[i] - 2 * b[i];
        }
        return ret;
    };

    // Determine the contracted point
    auto contr = [](const coord& a, const coord & b, size_t dimensions) {
        coord ret(dimensions);
        for (size_t i = 0; i < dimensions; ++i) {
            ret[i] = 0.5 * (a[i] + b[i]);
        }
        return ret;
    };

    // Simplex object with fitness value and the associated values for each dimension
    // greater is the sorting mechanism 
    multimap<double, coord, greater<double>> simplex;
    
    // Simplex step size
    double step = 0.5;
    
    // Extract values from the current continued fraction
    coord tmp(params);
    for (size_t i = 0; i < params; ++i) {
        tmp[i] = model->get_param(positions[i]);
    }

    // Centroid
    coord cent(params);      
    double cent_fit;

    // Evaluation of the simplex at the current point
    simplex.insert({model->get_fitness(), tmp});

    // Create simplex points by steping forward in each dimension a length of 'step' and appending
    // to the simplex. After this operation our simplex will be params+1 in length, as we have the original
    // fitness and values, then 10 modifications where each dimension is stepped
    for (size_t i = 0; i < params; ++i) {
        tmp[i] += step;
        double fitness = local_search::model_evaluate(tmp, positions, model, data, selected);
        simplex.insert({ fitness, tmp});
        tmp[i] -= step;
    }

    // Given our array of simplex points, calculate the centroid point and its fitness
    for (size_t i = 0; i < params; ++i) {
        cent[i] = 0;
        for (auto it = ++simplex.begin(); it != simplex.end(); ++it)

            // Sum the values from each dimension
            // Caclulate the average value on the fly to minimise risk of overflow
            cent[i] += it->second[i] / params;
        
    }
    cent_fit = local_search::model_evaluate(cent, positions, model, data, selected);

    size_t iter = 0;       // either converge, or reach max number of iterations, or stagnate for too long
    size_t stag = 0;       // stagnation is when the new vertex is still the worst
    double vtmp_fit = 0;
    
    while ( simplex.begin()->first - (--simplex.end())->first > numeric_limits<double>::epsilon()  &&   // The range in the simplex points has a difference > some epsilon   
            iter < meme::NELDER_MEAD_MOVES &&                           // We have not reached the max iterations
            stag < meme::NELDER_MEAD_STALE                              // We have not stagnated
        ) {
        

        // Get the worst point i.e. highest fiteness 
        coord& vw = simplex.begin()->second;
        double vw_fit = simplex.begin()->first;
        
        // Get the second worst point
        double vsw_fit = (++simplex.begin())->first;

        // Get the best point
        coord& vb = (--simplex.end())->second;

        // Get the fitness after reflection
        coord vr = refl(cent, vw, params);
        double vr_fit = local_search::model_evaluate(vr, positions, model, data, selected);

        // Get the point of expansion from the word and centroid points
        coord ve = expa(cent, vw, params);

        coord vtmp;
        // If reflection is better than our worst point, try expanding and
        // settle on the best result
        if (vr_fit < vw_fit) {

            if (local_search::model_evaluate(ve, positions, model, data, selected) < cent_fit) {      
                if (vr_fit < cent_fit)
                    vtmp = contr(vr, ve, params);
                else
                    vtmp = contr(refl(cent, vr, params), cent, params);
            } else
                vtmp = cent;

            vtmp = contr(contr(vtmp, cent, params), ve, params);

        } else {

            if (vsw_fit < cent_fit)
                vtmp = vb;
            else
                vtmp = cent;

            vector<double> ncvec = contr(refl(cent, ve, params), contr(vtmp, cent, params), params);
            if (local_search::model_evaluate(ncvec, positions, model, data, selected)
                    < cent_fit)
                vtmp = refl(cent, vr, params);
            else
                vtmp = cent;

            //moh: floating point comparison is used here
            if (local_search::model_evaluate(vtmp, positions, model, data, selected) < cent_fit)
                vtmp = vr;
            else
                vtmp = contr(cent, vw, params);

            vtmp = contr(vw, contr(vtmp, cent, params), params);
        }

        // replace the worst
        simplex.erase(simplex.begin());
        
        //moh: floating point comparison is used here
        
        vtmp_fit = local_search::model_evaluate(vtmp, positions, model, data, selected);

        if (vtmp_fit >= simplex.begin()->first) {
            stag++;
        }
        else {
            stag = 0;
        }
        
        simplex.insert({vtmp_fit, vtmp});

        // recompute the centroid
        for (size_t i = 0; i < params; ++i) {
            cent[i] = 0;
            for (auto it = ++simplex.begin(); it != simplex.end(); ++it)
                cent[i] += it->second[i] / params;
        }
        cent_fit = local_search::model_evaluate(cent, positions, model, data, selected);

        ++iter;
    }

    coord best_found = (--simplex.end())->second;

    for(size_t i = 0; i < best_found.size(); i++)
        model->get_param(positions[i]) = best_found[i];

    selected = vector<size_t>();
    Agent<U>::OBJECTIVE(model, data, selected);

    return model->get_fitness();
}

/**
 * Nedler-Mead "Algorithm 4" from "Evolving a Nelder–Mead Algorithm for Optimization with Genetic Programming"
 * 
 * @param model Model to optimise
 * @return double
 * 
 */
template <class U>
double local_search::custom_nelder_mead_alg4(U* model, DataSet* data, vector<size_t>& selected) {

    // "In order to find out whether the parts of the algorithm that are rarely executed matter at all, 
    //  we simplified the genetically bred Algorithm 3 in such a way that we. Nearly half of the code is
    //  hardly ever executed. We simplified the remaining, the result was a much simpler algorithm, shown
    //  under Algorithm 4"
    
    using coord = vector<double>;

    // Determine the number of parameters to optimise
    vector<size_t> positions = model->get_active_positions();
    size_t params = positions.size();
    /*for(size_t i = 0; i < model->get_count(); i++) {
        
        if (model->get_active(i))  {
            params++;
            positions.push_back(i);
        }
    }*/
        
    // Simplex object with fitness value and the associated values for each dimension
    // greater is the sorting mechanism 
    multimap<double, coord, greater<double>> simplex;
    
    // Simplex step size
    double step = 0.5;
    
    // Extract values from the current continued fraction
    coord tmp(params);
    for (size_t i = 0; i < params; ++i)
        tmp[i] = model->get_value(positions[i]);
    simplex.insert({model->get_fitness(), tmp});

    // Step in each direction of each dimension to make a new point
    for (size_t i = 0; i < params; ++i) {
        tmp[i] += step;
        double fitness = local_search::model_evaluate(tmp, positions, model, data, selected);
        simplex.insert({fitness, tmp});
        tmp[i] -= step;
    }

    // Determine the centroid of our simplex
    coord cent(params);      
    for (size_t i = 0; i < params; ++i) {
        
        // We skip the worst point as it is excluded from the centroid with ++ simplex.begin()
        cent[i] = 0;
        for (auto it = ++simplex.begin(); it != simplex.end(); ++it)
            // Caclulate the average value on the fly to minimise risk of overflow
            cent[i] += it->second[i] / params;
    }
    double cent_fit = local_search::model_evaluate(cent, positions, model, data, selected);

    size_t iter = 0;       // either converge, or reach max number of iterations, or stagnate for too long
    size_t stag = 0;       // stagnation is when the new vertex is still the worst
    
    while ( simplex.begin()->first - (--simplex.end())->first > numeric_limits<double>::epsilon()  &&   // The range in the simplex points has a difference > some epsilon   
            iter < meme::NELDER_MEAD_MOVES &&                           // We have not reached the max iterations
            stag < meme::NELDER_MEAD_STALE                              // We have not stagnated
        ) {

        coord vn;
        double vn_fit;

        // Get the worst point i.e. highest fiteness 
        coord& vw = simplex.begin()->second;
        double vw_fit = simplex.begin()->first;

        vector<double> c1_p1 = vo(cent, vw, minus<double>() );
        vector<double> c1_p2 = vo(cent, c1_p1, plus<double>() );
        double c1_fit = local_search::model_evaluate( c1_p2, positions, model, data, selected);

        if( c1_fit < vw_fit ) {

            vector<double> c2_p1 = vm(c1_p1, 2);
            vector<double> c2_p2 = vo(cent, c2_p1, plus<double>());
            double c2_fit = local_search::model_evaluate( c2_p2, positions, model, data, selected);
            if(c2_fit < cent_fit) {

                vector<double> vn_p1 = vm(c1_p1, 1.375);
                vector<double> vn_p2 = vo(cent, vn_p1, plus<double>());
                vn = vn_p2;

            } else
                vn = c1_p2;

        } else {
            
            vector<double> vn_p1 = vm(c1_p2, 0.625);
            vector<double> vn_p2 = vo(cent, vn_p1, minus<double>());
            vn = vn_p2;
        }

        vn_fit = local_search::model_evaluate( vn, positions, model, data, selected);
        
        // replace the worst
        simplex.erase(simplex.begin());
        simplex.insert({vn_fit, vn});
        
        // Track stagnant
        if (vn_fit >= simplex.begin()->first)       stag++;
        else                                        stag = 0;
        
        // recompute the centroid
        for (size_t i = 0; i < params; ++i) {
            cent[i] = 0;
            for (auto it = ++simplex.begin(); it != simplex.end(); ++it)
                cent[i] += it->second[i] / params;
        }

        cent_fit = local_search::model_evaluate(cent, positions, model, data, selected);
        ++iter;

    }

    coord best_found = (--simplex.end())->second;
    selected = vector<size_t>();
    local_search::model_evaluate(best_found, positions, model, data, selected);

    return model->get_fitness();
}

/**
 * Cusomised Nedler-Mead algorithm
 * 
 * @param model Model to optimise
 * @return double
 * 
 */
template <class U>
double local_search::custom_nelder_mead_redo(U* model, DataSet* data, vector<size_t>& selected) {

    using coord = vector<double>;

    // Determine the number of parameters to optimise
    vector<size_t> positions = model->get_active_positions();
    size_t ndim = positions.size();

    // Simplex object with fitness value and the associated values for each dimension
    // greater is the sorting mechanism 
    multimap<double, coord, greater<double>> simplex;
    
    // Simplex step size
    double step = 0.5;

    // Centroid
    coord cent(ndim);      
    double cent_fit;

    // Extract values from the current continued fraction
    coord tmp(ndim);
    for (size_t i = 0; i < ndim; ++i)
        tmp[i] = model->get_value(positions[i]);
    simplex.insert({model->get_fitness(), tmp});


    // Create simplex points by steping forward in each dimension a length of 'step' and appending
    // to the simplex. After this operation our simplex will be ndim+1 in length, as we have the original
    // fitness and values, then 10 modifications where each dimension is stepped
    for (size_t i = 0; i < ndim; ++i) {
        tmp[i] += step;
        simplex.insert({local_search::model_evaluate(tmp, positions, model, data, selected), tmp});
        tmp[i] -= step;
    }

    // Given our array of simplex points, calculate the centroid point and its fitness
    for (size_t i = 0; i < ndim; ++i) {
        cent[i] = 0;
        for (auto it = ++simplex.begin(); it != simplex.end(); ++it)

            // Sum the values from each dimension
            // Caclulate the average value on the fly to minimise risk of overflow
            cent[i] += it->second[i] / ndim;
        
    }

    cent_fit = local_search::model_evaluate(cent, positions, model, data, selected);

    size_t iter = 0;       // either converge, or reach max number of iterations, or stagnate for too long
    size_t stag = 0;       // stagnation is when the new vertex is still the worst

    while ( simplex.begin()->first - (--simplex.end())->first > numeric_limits<double>::epsilon()  &&   // The range in the simplex points has a difference > some epsilon   
            iter < meme::NELDER_MEAD_MOVES &&                           // We have not reached the max iterations
            stag < meme::NELDER_MEAD_STALE                              // We have not stagnated
        ) {
        
        // Get the worst point i.e. highest fiteness 
        coord& vw = simplex.begin()->second;
        double vw_fit = simplex.begin()->first;
        
        // Get the second worst point
        double vsw_fit = (++simplex.begin())->first;

        // Get the best point
        coord& vb = (--simplex.end())->second;

        // Get the fitness after reflection
        coord vr = refl(cent, vw);
        double vr_fit = local_search::model_evaluate(vr, positions, model, data, selected);       

        // Get the point of expansion from the word and centroid points
        coord ve = expa(cent, vw);

        coord vtmp;
        // If reflection is better than our worst point, try expanding and
        // settle on the best result
        if (vr_fit < vw_fit) {

            if (local_search::model_evaluate(ve, positions, model, data, selected) < cent_fit) {      
                if (vr_fit < cent_fit) {
                    vtmp = contr(vr, ve);
                } else {
                    vtmp = contr(refl(cent, vr), cent);
                }
            } else {
                vtmp = cent;
            }

            vtmp = contr(contr(vtmp, cent), ve);

        } else {

            if (vsw_fit < cent_fit) {
                vtmp = vb;
            } else {
                vtmp = cent;
            }

            vector<double> ncvec = contr(refl(cent, ve), contr(vtmp, cent));
            if (local_search::model_evaluate(ncvec, positions, model, data, selected) < cent_fit) {
                vtmp = refl(cent, vr);
            } else {
                vtmp = cent;
            }

            if (local_search::model_evaluate(vtmp, positions, model, data, selected) < cent_fit) {
                vtmp = vr;
            } else {
                vtmp = contr(cent, vw);
            }
            vtmp = contr(vw, contr(vtmp, cent));
        }

        // replace the worst
        simplex.erase(simplex.begin());
        
        double vtmp_fit = local_search::model_evaluate(vtmp, positions, model, data, selected);
        if (vtmp_fit >= simplex.begin()->first) ++stag;
        else stag = 0;

        simplex.insert({vtmp_fit, vtmp});

        // recompute the centroid
        for (size_t i = 0; i < ndim; ++i) {
            cent[i] = 0;
            for (auto it = ++simplex.begin(); it != simplex.end(); ++it) {
                double extra = it->second[i] / ndim;
                cent[i] += extra;
            }   
        }
        cent_fit = local_search::model_evaluate(cent, positions, model, data, selected);
        ++iter;
    }

    // Function on the full dataset if we were using partial in local search
    // otherwise we are using the performance of the limited optimisation
    selected = vector<size_t>();
    coord& vb = (--simplex.end())->second;
    local_search::model_evaluate(vb, positions, model, data, selected);      

    return model->get_fitness();
}

/**
 * Perform an operation on a vector
 * 
 * @param a         first vector
 * @param b         second vector
 * @param op        operator to apply to the two vectors
 * @return          vector of op(a,b)
 * 
 */
template <typename Operator>
vector<double> local_search::vo(vector<double> a, vector<double> b, Operator op) {
    vector<double> ret(a.size());
    transform(a.begin(), a.end(), b.begin(), ret.begin(), op);
    return ret; 
}

/**
 * Multiply a vector by a scalar
 * 
 * @param a         first vector
 * @param b         scalar
 * @return          vector of multiply(a,b)
 * 
 */
inline vector<double> local_search::vm(vector<double> a, double b) {
    vector<double> ret(a.size());
    transform(a.begin(), a.end(), ret.begin(), bind(multiplies<double>(), placeholders::_1, b));
    return ret; 
}

/**
 * Nelder Mead Reflection
 * 
 * @param a         first vector
 * @param b         second vector
 * @return          
 * 
 */
inline vector<double> local_search::refl(vector<double> a, vector<double> b) {
    return vo(vm(a, 2), b, minus<double>());
}

/**
 * Nelder Mead Expansion
 * 
 * @param a         first vector
 * @param b         second vector
 * @return          
 * 
 */
inline vector<double> local_search::expa(vector<double> a, vector<double> b) {
    return vo(vm(a,3), vm(b,2), minus<double>());
}

/**
 * Nelder Mead Contraction
 * 
 * @param a         first vector
 * @param b         second vector
 * @return          
 * 
 */
inline vector<double> local_search::contr(vector<double> a, vector<double> b) {
    return vm( vo(a,b,plus<double>()), 0.5);
}

/*
template <class T>
ostream& operator << (ostream& os, const vector<T>& v) 
{
    os << "[";
    for (typename vector<T>::const_iterator ii = v.begin(); ii != v.end(); ++ii)
    {
        os << " " << *ii;
    }
    os << "]";
    return os;
}
*/