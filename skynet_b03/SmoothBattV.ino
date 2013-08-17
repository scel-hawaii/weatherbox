/***************************************************
 *  Name:        sampleSmoothBatteryV
 *  Returns:     smooth battery voltage.
 *  Parameters:  raw unsmooth sample.
 *  Description: Takes the current sample, puts it into
                    a running average, and spits back out a 
                    smooth sample. This is primarily used 
                    for voltage detection during the 
                    power cutoff point.
 ***************************************************/
long SmoothBattV_init(void) {


}

long SmoothBattV_add(void) {


}

long SmoothBattV_get(void) {


}

long sampleSmoothBatteryV(int sample){
    double temp;
    /*
    smooth_batt_array[smooth_sample_index] = sample;

    // take the average of all the values
    for(i = 0 ; i < BATT_SAMPLE_SIZE ; i++)
        temp += smooth_batt_array[i];

    // Move the index up, and in a circle
    smooth_sample_index += 1 % 15;
    */

    //return (temp/BATT_SAMPLE_SIZE);
    return sample;
}
