
/**
 * @file
 * @author Andrew Ciezak <andy@ium.solutions>
 * @author Mohammad Haque <Mohammad.Haque@newcastle.edu.au>
 * @author Haoyuan Sun <hsun2@caltech.edu>
 * @version 1.0
 * @brief Implementation of objective functions
*/

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
double objective::mse(U* model, DataSet* train, vector<size_t>& selected ) {

    //auto t1 = chrono::high_resolution_clock::now();
    //model->show(cout, 18);

    try {

        double error_sum = 0;
        double error;
    
        if( selected.size() == 0) {

            
            for(size_t i = 0; i < train->count; i++) {

                double frac_val = model->evaluate(train->variables[i]);

                // Determine residual and square
                error = memetico::add(frac_val, -train->y[i]);
                //cout << "error: " << error;

                error = memetico::multiply(error, error);
                //cout << " error^2: " << error;

                // Weight squared error
                if(train->weight != nullptr)
                    error = memetico::multiply(error, train->weight[i]);

                // Sum of squared error
                error_sum = memetico::add(error_sum, error);
                //cout << " error_sum: " << error_sum << endl;

            }
        } else {

            for(size_t i : selected) {

                double frac_val = model->evaluate(train->variables[i]);

                // Determine residual and square
                error = memetico::add(frac_val, -train->y[i]);
                
                error = memetico::multiply(error, error);

                // Weight squared error
                if(train->weight != nullptr)
                    error = memetico::multiply(error, train->weight[i]);

                // Sum of squared error
                error_sum = memetico::add(error_sum, error);
            }
        }
        

        //model->show_min();
        //cout << memetico::PENALTY << endl;

        model->error = error_sum / train->count;
        model->penalty = 1+model->params_used()*memetico::PENALTY;
        model->fitness = memetico::multiply(model->error,model->penalty);
        //cout << " mse: " << model->error << endl;

    } catch (exception& e) {

        model->error = numeric_limits<double>::max();
        model->penalty = numeric_limits<double>::max();
        model->fitness = numeric_limits<double>::max();
        //cout << " exception!: " << e.what() << " error: " << model->error << " fitness: " << model->fitness << endl;

    }

    //auto t2 = chrono::high_resolution_clock::now();
    //chrono::duration<double, std::milli> ms_double = t2 - t1;
    //std::cout << " mse: " << ms_double.count() << "ms" << endl;

    return model->fitness;
    
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
double objective::nmse(U* model, DataSet* train, vector<size_t>& selected ) {

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

            for(size_t i = 0; i < train->count; i++) {

                predict = model->evaluate(train->variables[i]);

                // Add  target sums
                target_sum = memetico::add(target_sum, train->y[i]);

                // Determine error and square
                error = memetico::add(predict, -train->y[i]);
                error = memetico::multiply(error, error);    

                // Weight squared error
                if(train->weight != nullptr)
                    error = memetico::multiply(error, train->weight[i]);

                // Sum of squared error
                error_sum = memetico::add(error_sum, error);

            }

            mse = error_sum/train->count;
            target_avg = target_sum/train->count;

            // Determine variance
            for(size_t i = 0; i < train->count; i++) {                
                variance_sample = memetico::add(train->y[i],-target_avg);
                variance_sample = memetico::multiply(variance_sample,variance_sample);
                variance_sum = memetico::add(variance_sum, variance_sample);
            }
            variance = variance_sum/(train->count-1);

            model->error = mse/variance;
            model->penalty = 1+model->params_used()*memetico::PENALTY;
            model->fitness = memetico::multiply(model->error,model->penalty);

        } else {

            for(size_t i : selected) {

                predict = model->evaluate(train->variables[i]);

                // Add  target sums
                target_sum = memetico::add(train->y[i], target_sum);

                // Determine error and square
                error = memetico::add(predict, -train->y[i]);
                error = memetico::multiply(error, error);    

                // Weight squared error
                if(train->weight != nullptr)
                    error = memetico::multiply(error, train->weight[i]);

                // Sum of squared error
                error_sum = memetico::add(error_sum, error);             

            }

            mse = error_sum/train->count;
            target_avg = target_sum/train->count;

            // Determine variance
            for(size_t i : selected) {               
                variance_sample = memetico::add(train->y[i],-target_avg);
                variance_sum = memetico::add(variance_sum, variance_sample);
            }
            variance = variance_sum/(train->count-1);

            model->error = mse/variance;
            model->penalty = 1+model->params_used()*memetico::PENALTY;
            model->fitness = memetico::multiply(model->error,model->penalty);

        }

    } catch (exception& e) {

        model->error = numeric_limits<double>::max();
        model->penalty = numeric_limits<double>::max();
        model->fitness = numeric_limits<double>::max();

    }

    return model->fitness;

}
