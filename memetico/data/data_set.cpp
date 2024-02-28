
/**
 * @file
 * @author andy@impv.au
 * @version 1.0
 * @brief Implementation of the DataSet class
*/

#include <memetico/data/data_set.h>

vector<string> DataSet::IVS;

void DataSet::load() {

    // Ensure we can open file
    //cout << "Loading " << filename << endl;
    ifstream f;
    f.open(filename);
    if (!f.is_open())
        throw runtime_error("Unable to open file "+ filename);

    // Load data
    bool is_first = true;
    string line;
    while ( f >> line ) { 

        if(is_first)    load_header(line);
        else            load_data(line);

        is_first = false;
    }

    if( get_gpu() )
        setup_gpu();
}

void DataSet::load_header(string line) {
    
    stringstream ss(line);
    string part;

    DataSet::IVS.clear();

    size_t column = 0;
    while(ss.good()) {

        getline(ss, part, ',');    
        
        // Trim and remove line carrage if created from different os
        string word = trim(part);
        size_t index = word.find("\r", 0);
        if (index != string::npos)
            word.replace(index, 1, ""); 

        // Process element between commans
        if( word.compare("w") == 0 )                // If weight header
            weight_column = column;
        else if( word.compare("dy") == 0 )          // If uncertainty header
            uncertainty_column  = column;
        else if( word.compare("y") == 0 )           // If target header
            target_column = column;
        else if( word.compare("yd") == 0 ) {      // If derivative header
            derivative_column = column;
            if(meme::MAX_DER_ORD>=1) {
                Yder.push_back({});
                yder_min.push_back(0.0);
                yder_max.push_back(1.0);
            }
        }
        else if( word.compare("ydd") == 0 ) {     // If derivative header
            derivative2_column = column;
            if(meme::MAX_DER_ORD>=2) {
                Yder.push_back({});
                yder_min.push_back(0.0);
                yder_max.push_back(1.0);
            }
        }
        else if( word.compare("yddd") == 0 ) {     // If derivative header
            derivative3_column = column;
            if(meme::MAX_DER_ORD>=3) {
                Yder.push_back({});
                yder_min.push_back(0.0);
                yder_max.push_back(1.0);
            }
        }
        else                                    // else its a variable
            DataSet::IVS.push_back(word);    

        column++;
        
    }    
}

void DataSet::load_data(string line ) {

    stringstream ss(line);
    string part;

    size_t column = 0;
    vector<double> vars;
    while(ss.good()) {

        getline(ss, part, ',');    
        
        // Trim and remove line carrage if created from different os
        string word = trim(part);
        size_t index = word.find("\r", 0);
        if (index != string::npos)
            word.replace(index, 1, ""); 

        // Process element between commans
        if( column == weight_column )          
            weight.push_back(stod(word));
        else if( column == uncertainty_column )     
            dy.push_back(stod(word));
        else if( column == target_column )      
            y.push_back(stod(word));
        else if( column == derivative_column ) {
            if( meme::MAX_DER_ORD >= 1 )
                Yder[0].push_back(stod(word));
        }
        else if( column == derivative2_column ) {
            if( meme::MAX_DER_ORD >= 2 )
                Yder[1].push_back(stod(word));
        }
        else if( column == derivative3_column ) {
            if( meme::MAX_DER_ORD >= 3 )
                Yder[2].push_back(stod(word));
        }
        else           
            vars.push_back(stod(word));

        column++;
        
    } 
    samples.push_back(vars);

}

vector<size_t> DataSet::subset(float pct, bool to_GPU) {

    size_t ret_count = (long) (pct * get_count());
    vector<size_t> ret = RandInt::RANDINT->unique_set(ret_count, 0, get_count());

    if( gpu ) {

        // Free subset if already exists
        if(device_data.subset_size > 0) {
            freeSubset(&device_data);
            device_data.subset_size = 0;
        }

        // Copy new subset
        copySubset(&device_data, ret);
    }

    return ret;

}

/**
 * Output DataSet state
 * 
 * @param   out         Output stream to write to
 *                      Defaults to cout
 * 
 * @param   precision   resolution of real numerical output
 *                      Defaults to meme::PREC
 * 
 * @return          void
 */
/*
void DataSet::show(ostream& out, size_t precision) {

    size_t temp_precision = out.precision();
    out.precision(precision);

    size_t pad = precision+8;
    
    for(size_t i = 0; i < count; i++) {

        // Header
        if( i == 0) {
                
            out << setw(8) << "#" << setw(8) << "Sample" << setw(pad) << "y";
            for(size_t j = 0; j < ivs; j++)
                out << setw(pad) << names[j];
            
            if(has_uncertainty())
                out << setw(pad) << "dy";

            if(has_weight())
                out << setw(pad) << "weight";

            out << endl;
            
        }

        out << setw(8) << i+1;
        out << setw(8) << number[i];
        out << setw(pad) << y[i];

        for(size_t j = 0; j < ivs; j++)                    
            out << setw(pad) << variables[i][j];

        if(has_uncertainty())
            out << setw(pad) << dy[i];

        if(has_weight())
            out << setw(pad) << weight[i];
            
        out << endl;
    }

    cout << endl;
    out.precision(temp_precision);
    
}
*/

void DataSet::csv(string file_name) {

    ofstream f;
    f.open(file_name);
    if (!f.is_open())
        throw runtime_error("Unable to open file "+ file_name);
    
    for(size_t i = 0; i < get_count(); i++) {

        // Header
        if( i == 0) {
                
            f << "y";
            for(size_t j = 0; j < DataSet::IVS.size(); j++)
                f << "," << DataSet::IVS[j];
            
            if(has_uncertainty())
                f << "," << "dy";

            if(has_weight())
                f << "," << "weight";

            f << endl;
        }

        f << y[i];

        for(size_t j = 0; j < DataSet::IVS.size(); j++)                    
            f << ","  << samples[i][j];

        if(has_uncertainty())
            f << ","  << dy[i];

        if(has_weight())
            f << ","  << weight[i];
            
        f << endl;
    }  
}
