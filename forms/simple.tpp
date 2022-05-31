

/** 
 * @brief Randomise all active coeffents between +[min, max] or -[max, min]
 * @param min minimium value
 * @param max maximum value 
 * @return void
 * @bug should update the RANDINT/RANDREAL to be RAND and overload the function. That way we can use T min and T max as params
 * @bug the min max range removes the chance of being zero, but is really not suitable for the generic. There should just be min and max
 */
template <class T>
void Simple<T>::randomise(int min, int max) {

    this->active[0] = true;
    
    //  Set value with positive or negative value
    if( memetico::RANDREAL() < 0.5 )
        this->params[0] = memetico::RANDINT(min, max);
    else                               
        this->params[0] = memetico::RANDINT(max*-1, min*-1);
           
}
