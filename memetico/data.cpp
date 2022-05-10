
/**
 * @file
 * @author Andrew Ciezak <andy@ium.solutions>
 * @author Mohammad Haque <Mohammad.Haque@newcastle.edu.au>
 * @author Haoyuan Sun <hsun2@caltech.edu>
 * @version 1.0
 * @brief Implementation of the data class
*/

#include <memetico/data.h>

double          Data::SPLIT = 1;
size_t          Data::TRAIN_COUNT = 0;
size_t          Data::TEST_COUNT = 0;
size_t          Data::COUNT = 0;
vector<string>  Data::IVS = {};

string line;

/**
 * @brief
 * 
 * @param file
 * @param pct 
 *              Defaults to 1
 * 
 */
Data::Data(string filename, double pct) {

    // Only allow CSV
    if( !memetico::match_suffix(filename, string(".csv")) )
        throw runtime_error("Data file is not a CSV");
       
    // Ensure we can open file
    ifstream file(filename);
    if (!file.is_open())
        throw runtime_error("Data file is not a CSV");

    // Pre process file to extract header and sample count
    bool header = true;             // Flag for processing first row
    int dy_column = -1;             // Index of dy col
    int weight_column = -1;         // Index of weight col
    
    has_weight = false;
    has_dy = false;

    while ( file >> line ) {
        
        cout << line << endl;
        stringstream ss(line);

        if( header ) {
            
            size_t column = 0;
            string substr;
            while (ss.good()) {

                getline(ss, substr, ',');

                // Remove return character
                string word = memetico::trim(substr);
                size_t index = word.find("\r", 0);
                if (index != string::npos)
                    word.replace(index, 1, ""); 

                // If weight header
                if( word.compare("w") == 0 ) {
                    has_weight = true;
                    weight_column = column;
                }
                // If uncertainty header
                else if( word.compare("dy") == 0 ) {
                    has_dy = true;   
                    dy_column = column;
                }
                // If independent varaible header
                else if ( column != 0 ) {
                    Data::IVS.push_back(word);
                }
                column++;
            }

            header = false;
        } else 
            Data::COUNT++;

    }

    // Determine train/test split
    vector<size_t> selected_idx;
    if( pct < 1 ) {

        has_test = true;

        Data::TRAIN_COUNT = pct * Data::COUNT;
        Data::TEST_COUNT = Data::COUNT-Data::TRAIN_COUNT;

        selected_idx = memetico::RANDINT.unique_set(Data::TRAIN_COUNT, 0, Data::COUNT);

        train = new DataSet(Data::TRAIN_COUNT, Data::IVS.size(), has_dy, has_weight, Data::IVS);
        test = new DataSet(Data::TEST_COUNT, Data::IVS.size(), has_dy, has_weight, Data::IVS);
            
    } else {

        has_test = false;

        // Create the training dataset = testing dataset
        train = new DataSet(Data::COUNT, Data::IVS.size(), has_dy, has_weight, Data::IVS);
        Data::TRAIN_COUNT = Data::COUNT;
    }
    
    cout << " COUNT:" << Data::COUNT << " TRAIN: " << Data::TRAIN_COUNT << " TEST: " << Data::TEST_COUNT << endl;
    
    // Reset file
    file.clear();
    file.seekg(0);

    // Process samples
    header = true;
    size_t train_sample = 0;
    size_t test_sample = 0;
    
    for(size_t i = 0; i < Data::COUNT+1; i++) {

        file >> line;
        stringstream ss(line);
        
        size_t iv_pos = 0;

        // If header get indexes
        if( header ) {

            size_t column = 0;
            string substr;
            while (ss.good()) {

                getline(ss, substr, ',');
                string word = memetico::trim(substr);
                size_t index = word.find("\r", 0);
                if (index != string::npos)
                    word.replace(index, 1, ""); 

                // If weight header
                if( word.compare("w") == 0 ) {
                    has_weight = true;
                    weight_column = column;
                }
                // If uncertainty header
                else if( word.compare("dy") == 0 ) {
                    has_dy = true;   
                    dy_column = column;
                }
                column++;
            }
            header = false;

        } else {

            // Minus 1 for the header
            bool is_test = find(selected_idx.begin(), selected_idx.end(), i-1) == selected_idx.end();

            size_t column = 0;
            stringstream ss(line);
            string substr;
            while (ss.good()) {

                getline(ss, substr, ',');

                // If the index is 
                if( is_test ) {

                    // Process the column based on the column
                    if( column == 0 )                        test->y[test_sample] = stod(substr);
                    else if( int(column) == dy_column )      test->dy[test_sample] = stod(substr);
                    else if( int(column) == weight_column )  test->weight[test_sample] = stod(substr);
                    else {
                        test->variables[test_sample][iv_pos] = stod(substr);
                        iv_pos++;
                    }         
                    column++;

                } else {

                    // Process the column based on the column
                    if( column == 0 )                        train->y[train_sample] = stod(substr);
                    else if( int(column) == dy_column )      train->dy[train_sample] = stod(substr);
                    else if( int(column) == weight_column )  train->weight[train_sample] = stod(substr);
                    else {
                        train->variables[train_sample][iv_pos] = stod(substr);
                        iv_pos++;
                    }         
                    column++;

                }
            }

            // Increment counter for the sample and assign sample row number
            // This includes the header offset
            if( is_test ) {
                test->number[test_sample] = i;
                test_sample++;
            }
            else {
                train->number[train_sample] = i;
                train_sample++;
            }   
            
        }
    }
    
    // Close up
    file.close();

    show();
}

/**
 * @brief
 * 
 * @param train
 * @param test
 * 
 */
Data::Data(string train_file, string test_file) {

    // Only allow CSV
    if( !memetico::match_suffix(train_file, string(".csv")) )
        throw runtime_error("Train file is not a CSV");

    if( !memetico::match_suffix(test_file, string(".csv")) )
        throw runtime_error("Test file is not a CSV");

    // Extract meta data
    read_meta_data(train_file, false);
    read_meta_data(test_file, true);

    // Create space for train and test data
    train = new DataSet(Data::TRAIN_COUNT, Data::IVS.size(), has_dy, has_weight, Data::IVS);
    test = new DataSet(Data::TEST_COUNT, Data::IVS.size(), has_dy, has_weight, Data::IVS);

    Data::COUNT = Data::TRAIN_COUNT+Data::TEST_COUNT;

    read_samples(train_file, false);
    read_samples(test_file, true);

    has_test = true;

    show();

}

/**
 * Output Data state
 * 
 * @param   out         Output stream to write to
 *                      Defaults to cout
 * 
 * @param   precision   resolution of real numerical output
 *                      Defaults to memetico::PREC
 * 
 * @return          void
 */
void Data::show(ostream& out, size_t precision) {

    cout << "Train Data" << endl;
    cout << "-----------" << endl;

    train->show(out,precision);

    if(has_test) {
        cout << "Test Data" << endl;
        cout << "-----------" << endl;
        test->show(out,precision);
    }

}

/**
 * @brief extract headers and data counts prior to processing data
 * 
 * - Updates Data::TRAIN_COUNT or Data::TEST_COUNT base on is_test with the  
 * - Updates has_weight, has_dy based on the headers
 * 
 * @param   filename name of file to examine
 * @param   is_test true if we are examining test datafile
 * 
 * @return  void
 */
void Data::read_meta_data(string filename, bool is_test) {

    // File checks
    ifstream file(filename);
    if (!file.is_open())
        throw runtime_error("Data file is not a CSV");

    // Pre process file to extract header and sample count
    bool header = true;     // Flag for processing first row
    dy_col = 0;             // Index of dy col
    weight_col = -1;        // Index of weight col
    
    has_weight = false;
    has_dy = false;

    while ( file >> line ) {
        
        stringstream ss(line);

        if( header ) {
            
            // Only read in header information from training file
            if( is_test ) {

                size_t column = 0;
                string substr;
                while (ss.good()) {

                    getline(ss, substr, ',');

                    // Remove return character
                    string word = memetico::trim(substr);
                    size_t index = word.find("\r", 0);
                    if (index != string::npos)
                        word.replace(index, 1, ""); 

                    // If weight header
                    if( word.compare("w") == 0 ) {
                        has_weight = true;
                        weight_col = column;
                    }
                    // If uncertainty header
                    else if( word.compare("dy") == 0 ) {
                        has_dy = true;   
                        dy_col = column;
                    }
                    // If independent varaible header
                    else if ( column != 0 ) {
                        Data::IVS.push_back(word);
                    }
                    column++;
                }
            }

            header = false;
        } else 

            if(is_test)
                Data::TEST_COUNT++;
            else 
                Data::TRAIN_COUNT++;
    }

    file.close();
}

/**
 * @brief extract samples from a train or test file
 * 
 * - Assumes read_meta_data has been called or that
 * -- Data:TRAIN_COUNT or Data::TEST_COUNT are correct
 * -- Data:IVS is correctly configured for the file
 * -- train and test DataSets have been constructed
 * - Injects data from the file into train and test DataSets
 * 
 * @param   filename name of file to examine
 * @param   is_test true if we are examining test datafile
 * 
 * @return  void
 */
void Data::read_samples(string filename, bool is_test) {

    // Ensure we can open file
    ifstream file(filename);
    if (!file.is_open())
        throw runtime_error("Data file is not a CSV");

    size_t sample = 0;
    bool header = true;

    // Get reference to the dataset we are updating
    DataSet* ds;
    if( is_test )   ds = test;
    else            ds = train;

    while ( file >> line ) {
        
        stringstream ss(line);
        if( header ) {
            header = false;
        } else {

            size_t iv_pos = 0;
            size_t column = 0;
            stringstream ss(line);
            string substr;

            while (ss.good()) {

                getline(ss, substr, ',');
                string word = memetico::trim(substr);
                size_t index = word.find("\r", 0);
                if (index != string::npos)
                    word.replace(index, 1, ""); 

                // Process the column based on the column
                if( column == 0 )                   ds->y[sample] = stod(substr);
                else if( column == dy_col )         ds->dy[sample] = stod(substr);
                else if( column == weight_col )     ds->weight[sample] = stod(substr);
                else {
                    ds->variables[sample][iv_pos] = stod(substr);
                    iv_pos++;
                }         
                column++;

            }

            // Increment counter for the sample and assign sample row number
            // This includes the header offset
            ds->number[sample] = sample;
            sample++;
               
        }
    }

    // Close up
    file.close();
}