/****************************************************
 *
 *          SmoothBattV.ino
 *          REIS Weatherbox - Smooth Battery Voltage Functions
 *
 *          This file contains the functions for a moving 
 *          average voltage filter. The filter is used to 
 *          calculate the cutoff voltages for the low battery check,
 *          since our ADC input has a lot of noise. 
 *      
 *
 ****************************************************/

const int MOVING_AVG_SIZE = 15;
long smooth_batt_array[MOVING_AVG_SIZE];
long smooth_batt_mv;

void SmoothBattV_init(void) {
    for(int k = 0; k < MOVING_AVG_SIZE ; k++) {
        smooth_batt_array[k] = sampleBatteryVoltage(); 
    }
}

void SmoothBattV_add(void) {
    long temp[MOVING_AVG_SIZE];

    for(int k = 0; k < MOVING_AVG_SIZE-1 ; k++) {
        temp[k] = smooth_batt_array[k+1];
    }
    temp[MOVING_AVG_SIZE] = sampleBatteryVoltage();
}

long SmoothBattV_get(void) {
    long temp;
    for(int k = 0; k < MOVING_AVG_SIZE ; k++) {
        temp += sampleBatteryVoltageRaw();
    }
    return (long)((temp/MOVING_AVG_SIZE) * 5000/1023);
}

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
