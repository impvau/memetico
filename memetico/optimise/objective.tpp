
/**
 * @file
 * @author Andrew Ciezak <andy@impv.au>
 * @version 1.0
 * @brief Implementation of objective functions
*/

using namespace meme;

/**
 * Mean Sqaure Error objective function
 * 
 * @param model Model to evaluate
 * @param train DataSet to determine error on
 * @param selected subset of data to evaluate. Empty subset indicates usage of all data
 * @return double
 * 
 */
template <class U>
double objective::mse(MemeticModel<U>* model, DataSet* train, vector<size_t>& selected ) {

    auto start = chrono::system_clock::now();

    if( !train->get_gpu() ) {

        try {

            double error_sum = 0;
            double error;
        
            if( selected.size() == 0) {
                
                for(size_t i = 0; i < train->get_count(); i++) {

                    double frac_val = model->evaluate(train->samples[i]);

                    // Determine residual and square
                    error = add(frac_val, -train->y[i]);

                    error = multiply(error, error);

                    // Weight squared error
                    if(train->has_weight())
                        error = multiply(error, train->weight[i]);

                    // Sum of squared error
                    error_sum = add(error_sum, error);

                }

                model->set_error(error_sum / train->get_count());

            } else {

                for(size_t i : selected) {

                    double frac_val = model->evaluate(train->samples[i]);

                    // Determine residual and square
                    error = add(frac_val, -train->y[i]);
                    
                    error = multiply(error, error);

                    // Weight squared error
                    if(train->has_weight())
                        error = multiply(error, train->weight[i]);

                    // Sum of squared error
                    error_sum = add(error_sum, error);
                }
                
                model->set_error(error_sum / selected.size());

            }
            
            model->set_penalty( 1+model->get_count_active()*meme::PENALTY );
            model->set_fitness( multiply(model->get_error(),model->get_penalty()) );

        } catch (exception& e) {

            model->set_error(numeric_limits<double>::max());
            model->set_penalty(numeric_limits<double>::max());
            model->set_fitness(numeric_limits<double>::max());

        }

    } else  {

        model->set_error( cuda_error(model, train, selected) );
        model->set_penalty( 1+model->get_count_active()*meme::PENALTY );
        model->set_fitness( multiply(model->get_error(),model->get_penalty()) );
    }
    
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> ms = end-start;
    
    return model->get_fitness();
}

template <class U>
double objective::mae(MemeticModel<U>* model, DataSet* train, vector<size_t>& selected ) {

    auto start = chrono::system_clock::now();

    if( !train->get_gpu() ) {

        try {

            double error_sum = 0;
            double error;
        
            if( selected.size() == 0) {
                
                for(size_t i = 0; i < train->get_count(); i++) {

                    double frac_val = model->evaluate(train->samples[i]);

                    // Determine residual and square
                    error = add(frac_val, -train->y[i]);
                    error = fabs(error);

                    // Weight squared error
                    if(train->has_weight())
                        error = multiply(error, train->weight[i]);

                    // Sum of squared error
                    error_sum = add(error_sum, error);

                }

                model->set_error(error_sum / train->get_count());

            } else {

                for(size_t i : selected) {

                    double frac_val = model->evaluate(train->samples[i]);

                    // Determine residual and square
                    error = add(frac_val, -train->y[i]);
                    error = fabs(error);

                    // Weight squared error
                    if(train->has_weight())
                        error = multiply(error, train->weight[i]);

                    // Sum of squared error
                    error_sum = add(error_sum, error);
                }
                
                model->set_error(error_sum / selected.size());

            }
            
            model->set_penalty( 1+model->get_count_active()*meme::PENALTY );
            model->set_fitness( multiply(model->get_error(),model->get_penalty()) );

        } catch (exception& e) {

            model->set_error(numeric_limits<double>::max());
            model->set_penalty(numeric_limits<double>::max());
            model->set_fitness(numeric_limits<double>::max());

        }

    } else  {

        model->set_error( cuda_error(model, train, selected, metric_t::mean_absolute_error) );
        model->set_penalty( 1+model->get_count_active()*meme::PENALTY );
        model->set_fitness( multiply(model->get_error(),model->get_penalty()) );
    }
    
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> ms = end-start;
    
    return model->get_fitness();

}

template <class U>
double objective::rmse(MemeticModel<U>* model, DataSet* train, vector<size_t>& selected ) {

    mse(model, train, selected);

    try {

        model->set_error( sqrt(model->get_error()) );
        model->set_fitness( multiply(model->get_error(),model->get_penalty()));

    } catch (exception& e) {

        model->set_error(numeric_limits<double>::max());
        model->set_penalty(numeric_limits<double>::max());
        model->set_fitness(numeric_limits<double>::max());

    }

    return model->get_fitness();

}

template <class U>
double objective::cuda_error(MemeticModel<U>* model, DataSet* train, vector<size_t>& selected, metric_t metric) {

    // Convert CFR to Program
    Program p;
    TreeNode *tn_frac = new TreeNode();
    model->get_node(tn_frac);
    cout << model << endl;
    get_prefix(p.prefix, tn_frac);
    p.length = p.prefix.size();

    // Setup call to GPU Calculate Fitness
    vector<Program> pop;
    pop.push_back(p);
    int blockNum;
    
    if(selected.size() == 0) {
        blockNum = (train->get_count() - 1) / THREAD_PER_BLOCK + 1;
        train->device_data.subset_size = 0;
    } else
        blockNum = (selected.size() - 1) / THREAD_PER_BLOCK + 1;


    double ret = cusr::calculateFitness(train->device_data, blockNum, pop, metric);
    delete tn_frac;
    return ret;

}

/**
 * Normalised Mean Sqaure Error objective function
 * 
 * @param model Model to evaluate
 * @param train DataSet to determine error on
 * @param selected subset of data to evaluate. Empty subset indicates usage of all data
 * @return double
 * 
 */
template <class U>
double objective::nmse(MemeticModel<U>* model, DataSet* train, vector<size_t>& selected ) {

    try {

        // Basic vars
        double error_sum = 0;       // Sum of errors for all samples
        double target_sum = 0;      // Sum of target values for all samples
        double error;               // Error for a single sample
        double predict;             // Prediction for a single sample

        double target_avg;          // Average target value

        // Variance
        double variance_sample;     // Variance for a single sample
        double variance_sum = 0;    // Sum of variance for all samples
        double variance;            // Variance given model and train data

        double mse;                 // MSE given model and train data

        if( selected.size() == 0) {

            for(size_t i = 0; i < train->get_count(); i++) {

                predict = model->evaluate(train->samples[i]);

                // Add  target sums
                target_sum = add(target_sum, train->y[i]);

                // Determine error and square
                error = add(predict, -train->y[i]);
                error = multiply(error, error);    

                // Weight squared error
                if(train->has_weight())
                    error = multiply(error, train->weight[i]);

                // Sum of squared error
                error_sum = add(error_sum, error);

            }

            mse = error_sum/train->get_count();
            target_avg = target_sum/train->get_count();

            // Determine variance
            for(size_t i = 0; i < train->get_count(); i++) {                
                variance_sample = add(train->y[i],-target_avg);
                variance_sample = multiply(variance_sample,variance_sample);
                variance_sum = add(variance_sum, variance_sample);
            }
            variance = variance_sum/(train->get_count()-1);

            model->set_error(mse/variance);
            model->set_penalty(1+model->get_count_active()*meme::PENALTY);
            model->set_fitness(multiply(model->get_error(),model->get_penalty()));

        } else {

            for(size_t i : selected) {

                predict = model->evaluate(train->samples[i]);

                // Add  target sums
                target_sum = add(train->y[i], target_sum);

                // Determine error and square
                error = add(predict, -train->y[i]);
                error = multiply(error, error);    

                // Weight squared error
                if(train->has_weight())
                    error = multiply(error, train->weight[i]);

                // Sum of squared error
                error_sum = add(error_sum, error);             

            }

            mse = error_sum/train->get_count();
            target_avg = target_sum/train->get_count();

            // Determine variance
            for(size_t i : selected) {               
                variance_sample = add(train->y[i],-target_avg);
                variance_sum = add(variance_sum, variance_sample);
            }
            variance = variance_sum/(train->get_count()-1);

            model->set_error(mse/variance);
            model->set_penalty(1+model->get_count_active()*meme::PENALTY);
            model->set_fitness(multiply(model->get_error(),model->get_penalty()));

        }

    } catch (exception& e) {

        model->set_error(numeric_limits<double>::max());
        model->set_penalty(numeric_limits<double>::max());
        model->set_fitness(numeric_limits<double>::max());

    }

    return model->get_fitness();

}

template <class U>
double objective::compare(MemeticModel<U>* m1, MemeticModel<U>* m2, DataSet* train) {

    double error_dist = 0;
    double err1, err2;

    try {

        for(size_t i = 0; i < train->get_count(); i++) {

            double m1_frac_val = m1->evaluate(train->samples[i]);
            double m2_frac_val = m2->evaluate(train->samples[i]);
            
            // Determine residual and square
            err1 = add(m1_frac_val, -train->y[i]);
            err2 = add(m2_frac_val, -train->y[i]);
            
            // Square error
            err1 = multiply(err1, err1);
            err2 = multiply(err2, err2);
            
            // Sum of squared error
            error_dist = add(error_dist, fabs(err1-err2));
            
        }
      
    } catch (exception& e) {
        return numeric_limits<double>::max();    
    }
        
    return error_dist;
}