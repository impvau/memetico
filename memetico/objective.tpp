
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

        model->set_error( error_sum / train->count );
        model->set_penalty( 1+model->get_count_active()*memetico::PENALTY );
        model->set_fitness( memetico::multiply(model->get_error(),model->get_penalty()) );
        //cout << " mse: " << model->get_error() << endl;

    } catch (exception& e) {

        model->set_error(numeric_limits<double>::max());
        model->set_penalty(numeric_limits<double>::max());
        model->set_fitness(numeric_limits<double>::max());
        //cout << " exception!: " << e.what() << " error: " << model->get_error() << " fitness: " << model->fitness << endl;

    }

    //auto t2 = chrono::high_resolution_clock::now();
    //chrono::duration<double, std::milli> ms_double = t2 - t1;
    //std::cout << " mse: " << ms_double.count() << "ms" << endl;

    return model->get_fitness();
    
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

            model->set_error(mse/variance);
            model->set_penalty(1+model->get_count_active()*memetico::PENALTY);
            model->set_fitness(memetico::multiply(model->get_error(),model->get_penalty()));

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

            model->set_error(mse/variance);
            model->set_penalty(1+model->get_count_active()*memetico::PENALTY);
            model->set_fitness(memetico::multiply(model->get_error(),model->get_penalty()));

        }

    } catch (exception& e) {

        model->set_error(numeric_limits<double>::max());
        model->set_penalty(numeric_limits<double>::max());
        model->set_fitness(numeric_limits<double>::max());

    }

    return model->get_fitness();

}

template <class U>
double objective::compare(U* m1, U* m2, DataSet* train) {

    double error_dist = 0;
    double err1, err2;

    try {

        for(size_t i = 0; i < train->count; i++) {

            double m1_frac_val = m1->evaluate(train->variables[i]);
            double m2_frac_val = m2->evaluate(train->variables[i]);
            
            // Determine residual and square
            err1 = memetico::add(m1_frac_val, -train->y[i]);
            err2 = memetico::add(m2_frac_val, -train->y[i]);
            
            // Square error
            err1 = memetico::multiply(err1, err1);
            err2 = memetico::multiply(err2, err2);
            
            // Sum of squared error
            error_dist = memetico::add(error_dist, fabs(err1-err2));
            
        }
      
    } catch (exception& e) {

        cout << "compare math error " << endl;
        return numeric_limits<double>::max();
    
    }
        
    return error_dist;
}