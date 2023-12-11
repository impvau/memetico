
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

            double weight_sum = 0;
            double error_sum = 0;
            double error;
        
            if( selected.size() == 0) {
                
                for(size_t i = 0; i < train->get_count(); i++) {

                    double frac_val = model->evaluate(train->samples[i]);

                    // Determine residual and square
                    error = add(frac_val, -train->y[i]);

                    error = multiply(error, error);

                    // Weight squared error
                    if(train->has_weight()) {
                        error = multiply(error, train->weight[i]);
                        weight_sum += train->weight[i];
                    }

                    // Sum of squared error
                    error_sum = add(error_sum, error);

                }

                // After the loop, use weight_sum to calculate the average error
                if(weight_sum > 0)  model->set_error(error_sum / weight_sum);
                else                model->set_error(error_sum / train->get_count());

            } else {

                for(size_t i : selected) {

                    double frac_val = model->evaluate(train->samples[i]);

                    // Determine residual and square
                    error = add(frac_val, -train->y[i]);
                    
                    error = multiply(error, error);

                    // Weight squared error
                    if(train->has_weight()) {
                        error = multiply(error, train->weight[i]);
                        weight_sum += train->weight[i];
                    }

                    // Sum of squared error
                    error_sum = add(error_sum, error);
                }
                
                // After the loop, use weight_sum to calculate the average error
                if(weight_sum > 0)  model->set_error(error_sum / weight_sum);
                else                model->set_error(error_sum / train->get_count());
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
double objective::mape(MemeticModel<U>* model, DataSet* train, vector<size_t>& selected) {

    auto start = chrono::system_clock::now();

    if( !train->get_gpu() ) {

        try {

            double error_sum = 0;
            double error;
        
            if( selected.size() == 0) {
                
                for(size_t i = 0; i < train->get_count(); i++) {

                    double pred_val = model->evaluate(train->samples[i]);

                    // Calculate absolute percentage error
                    if (train->y[i] != 0) { // Avoid division by zero
                        error = fabs((pred_val - train->y[i]) / train->y[i]);
                    } else {
                        error = 0; // Handle zero actual value case
                    }

                    // Weight error
                    if(train->has_weight())
                        error = multiply(error, train->weight[i]);

                    // Sum of errors
                    error_sum = add(error_sum, error);

                }

                model->set_error(error_sum / train->get_count());

            } else {

                for(size_t i : selected) {

                    double pred_val = model->evaluate(train->samples[i]);

                    // Calculate absolute percentage error
                    if (train->y[i] != 0) {
                        error = fabs((pred_val - train->y[i]) / train->y[i]);
                    } else {
                        error = 0;
                    }

                    // Weight error
                    if(train->has_weight())
                        error = multiply(error, train->weight[i]);

                    // Sum of errors
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

        throw std::runtime_error("GPU branch not implemented for MAPE");

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

template <class U>
double objective::p_cor(MemeticModel<U>* model, DataSet* train, vector<size_t>& selected) {

        auto start = chrono::system_clock::now();

    try {
        double pearson_correlation;
        const double epsilon = 1e-5; // Small threshold for variance

        if (train->has_weight()) {
            double sum_weight = 0;
            double weighted_mean_x = 0, weighted_mean_y = 0;
            double weighted_m2_x = 0, weighted_m2_y = 0, weighted_crossproduct = 0;

            auto calculateWeightedCorrelation = [&](size_t i) {
                double x = model->evaluate(train->samples[i]);
                double y = train->y[i];
                double weight = train->weight[i];
                sum_weight += weight;

                double delta_x = x - weighted_mean_x;
                double delta_y = y - weighted_mean_y;
                weighted_mean_x += (delta_x * weight) / sum_weight;
                weighted_mean_y += (delta_y * weight) / sum_weight;
                weighted_m2_x += weight * delta_x * (x - weighted_mean_x);
                weighted_m2_y += weight * delta_y * (y - weighted_mean_y);
                weighted_crossproduct += weight * delta_x * (y - weighted_mean_y);
            };

            if (selected.size() == 0) {
                for (size_t i = 0; i < train->get_count(); i++) {
                    calculateWeightedCorrelation(i);
                }
            } else {
                for (size_t i : selected) {
                    calculateWeightedCorrelation(i);
                }
            }

            double weighted_variance_x = weighted_m2_x / sum_weight;
            double weighted_variance_y = weighted_m2_y / sum_weight;
            double weighted_covariance = weighted_crossproduct / sum_weight;

            if (weighted_variance_x < epsilon || weighted_variance_y < epsilon) {
                model->set_error(1);
                model->set_penalty(1);
                model->set_fitness(1);
                return model->get_fitness();
            }

            pearson_correlation = weighted_covariance / sqrt(weighted_variance_x * weighted_variance_y);

        } else {
            size_t n = 0;
            double mean_x = 0, mean_y = 0;
            double m2_x = 0, m2_y = 0, crossproduct = 0;

            auto calculateCorrelation = [&](size_t i) {
                double x = model->evaluate(train->samples[i]);
                double y = train->y[i];
                n++;
                double delta_x = x - mean_x;
                double delta_y = y - mean_y;
                mean_x += delta_x / n;
                mean_y += delta_y / n;
                m2_x += delta_x * (x - mean_x);
                m2_y += delta_y * (y - mean_y);
                crossproduct += delta_x * (y - mean_y);
            };

            if (selected.size() == 0) {
                for(size_t i = 0; i < train->get_count(); i++) {
                    calculateCorrelation(i);
                }
            } else {
                for (size_t i : selected) {
                    calculateCorrelation(i);
                }
            }  

            double variance_x = m2_x / n;
            double variance_y = m2_y / n;

            if (variance_x < epsilon || variance_y < epsilon) {
                model->set_error(1);
                model->set_penalty(1);
                model->set_fitness(1);
                return model->get_fitness();
            }

            pearson_correlation = crossproduct / n / sqrt(variance_x * variance_y);
        }

        if (abs(pearson_correlation) > 1) {
            pearson_correlation = 1;
        }

        model->set_error(1 - abs(pearson_correlation));
        model->set_penalty(1+model->get_count_active()*meme::PENALTY);
        model->set_fitness(1 - abs(pearson_correlation/model->get_penalty()) );

    } catch (const std::exception& e) {
        model->set_error(1);
        model->set_penalty(1);
        model->set_fitness(1);
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> ms = end - start;

    if (std::isnan(model->get_error())) {
        model->set_error(1);
        model->set_penalty(1);
        model->set_fitness(1);
    }

    return model->get_fitness();

}

template <class U>
double objective::s_cor(MemeticModel<U>* model, DataSet* train, vector<size_t>& selected) {

    auto start = chrono::system_clock::now();

    vector<double> x_values;
    vector<double> y_values;

    // Populate x_values and y_values
    if (selected.size() == 0) {
        for(size_t i = 0; i < train->get_count(); i++) {
            x_values.push_back(model->evaluate(train->samples[i]));
            y_values.push_back(train->y[i]);
        }
    } else {
        for (size_t i : selected) {
            x_values.push_back(model->evaluate(train->samples[i]));
            y_values.push_back(train->y[i]);
        }
    }

    // Get ranks for x and y values
    vector<double> x_ranks = s_rank(x_values);
    vector<double> y_ranks = s_rank(y_values);

    size_t n = x_ranks.size();
    double mean_x = 0, mean_y = 0;
    double m2_x = 0, m2_y = 0, crossproduct = 0;

    for (size_t i = 0; i < n; ++i) {
        double x = x_ranks[i];
        double y = y_ranks[i];
        double delta_x = x - mean_x;
        double delta_y = y - mean_y;
        mean_x += delta_x / (i + 1);
        mean_y += delta_y / (i + 1);
        m2_x += delta_x * (x - mean_x);
        m2_y += delta_y * (y - mean_y);
        crossproduct += delta_x * (y - mean_y);
    }

    double variance_x = m2_x / n;
    double variance_y = m2_y / n;

    // Check for near-constant ranks
    const double epsilon = 1e-8;  // Small threshold for variance
    if (variance_x < epsilon || variance_y < epsilon) {
        model->set_error(1);
        model->set_penalty(1);
        model->set_fitness(1);
        return model->get_fitness();
    }

    double covariance = crossproduct / n;
    double spearman_correlation = covariance / sqrt(variance_x * variance_y);

    if (abs(spearman_correlation) > 1) {
        return 1;
    }

    model->set_error(1 - abs(spearman_correlation));
    model->set_penalty(1);
    model->set_fitness(1 - abs(spearman_correlation));

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> ms = end-start;

    return model->get_fitness();
}

// Helper function to compute ranks
inline vector<double> objective::s_rank(vector<double>& data) {
    vector<size_t> indices(data.size());
    iota(indices.begin(), indices.end(), 0);

    sort(indices.begin(), indices.end(), [&data](size_t a, size_t b) { return data[a] < data[b]; });

    vector<double> ranks(data.size());
    for (size_t i = 0; i < data.size(); i++) {
        ranks[indices[i]] = i + 1;
    }

    return ranks;
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