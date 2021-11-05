
/**
 * @file
 * @author Andrew Ciezak <andy@ium.solutions>
 * @author Mohammad Haque <Mohammad.Haque@newcastle.edu.au>
 * @author Haoyuan Sun <hsun2@caltech.edu>
 * @version 1.0
 * @brief Implementation of local search functions
*/
template <class U>
double local_search::model_evaluate(vector<double> params, vector<size_t> positions, U* model, DataSet* data, vector<size_t>& selected) {

    for(size_t i = 0; i < params.size(); i++)
        model->params[positions[i]] = params[i];
    
    return Agent<U>::OBJECTIVE(model, data, selected);

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

    local_search::nm_duration = 0;
    local_search::nm_iters = 0;

    auto start_nm = chrono::high_resolution_clock::now();

    if( memetico::DEBUG )
        cout << "NelderMead" << endl;

    using coord = vector<double>;

    // Determine the number of parameters to optimise
    size_t params = 0;
    vector<size_t> positions;
    for(size_t i = 0; i < model->count; i++) {
        
        if (model->active[i])  {
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
    //cout << "Extracting Fraction" << endl;
    coord tmp(params);
    for (size_t i = 0; i < params; ++i) {
        tmp[i] = model->params[positions[i]];
        //cout << tmp[i] << endl;
    }

    // Centroid
    coord cent(params);      
    double cent_fit;

    // Evaluation of the simplex at the current point
    simplex.insert({model->fitness, tmp});

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
            cent[i] += it->second[i] /= params;
        
    }
    cent_fit = local_search::model_evaluate(cent, positions, model, data, selected);

    size_t iter = 0;       // either converge, or reach max number of iterations, or stagnate for too long
    size_t stag = 0;       // stagnation is when the new vertex is still the worst
    double vtmp_fit = 0;
    
    while ( simplex.begin()->first - (--simplex.end())->first > numeric_limits<double>::epsilon()  &&   // The range in the simplex points has a difference > some epsilon   
            iter < memetico::NELDER_MEAD_MOVES &&                           // We have not reached the max iterations
            stag < memetico::NELDER_MEAD_STALE                              // We have not stagnated
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
            //cout << "Increasing stag count to " << stag << endl;
        }
        else {
            //cout << "Improvement Found, resetting stag count " << endl;
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

    //if( iter == memetico::NELDER_MEAD_MOVES )
    //    cout << "Returning on iteration " << iter << " due to max moves " << memetico::NELDER_MEAD_MOVES <<  " reached " << endl;

    //if( stag == memetico::NELDER_MEAD_STALE )
    //    cout << "Returning on iteration " << iter << " due to stale " << endl;

    //cout << "returning " << vtmp_fit << endl;

    auto end_nm = chrono::high_resolution_clock::now();
    local_search::nm_duration = chrono::duration_cast<chrono::milliseconds>(end_nm-start_nm).count();
    local_search::nm_iters = iter;
    //cout << "dur: " << nm_duration << " iter: " << iter << " best: " << (--simplex.end())->first << " worst: " << simplex.begin()->first << endl;

    //cout << "BestFit: " << (--simplex.end())->first << " WorstFit: " << simplex.begin()->first << " FoundFit: " << vtmp_fit << endl;

    coord best_found = (--simplex.end())->second;

    for(size_t i = 0; i < best_found.size(); i++)
        model->params[positions[i]] = best_found[i];

    selected = vector<size_t>();
    Agent<U>::OBJECTIVE(model, data, selected);

    return model->fitness;
}
