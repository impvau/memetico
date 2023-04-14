
/**
 * @file
 * @author Andrew Ciezak <andy@impv.au>
 * @version 1.0
 * @brief Implementation of the Stat class
 */

/**
 * Construct Stat
 * - Assign count
 * - Assign names
 * - Assign display
 * - Assign number of stats based on names array
 * - Allocate space for values
 * 
 * @param stat_names names of stats we are tracking
 *  
 * @return  Agent
 */
Stat::Stat(vector<string> stat_names, vector<string> stat_display) {

    names = stat_names;
    display = stat_display;
    count = names.size();

    if( count != 0)
        values = (double *)malloc(count*sizeof(double));
    else
        throw runtime_exception("Can't create stats object with no elements");
    

}

Stat::~Stat() {

    if( count > 0 )
        delete stats;

}


void Stat::show(ostream& out, size_t precision) {


}