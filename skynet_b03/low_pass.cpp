#include "low_pass.h"
/* Initialize the filter */
void LPF_filter_init(LowPassFilter* f, float o, float a){
    f->output = o;
    f->alpha = a;
}

/* Updates the filter with another value */
float LPF_update_filter(LowPassFilter* f, float new_value){
    /* printf("Got a new datapoint %f\n", new_value); */
    // Output = alpha*new_value + (1 - alpha)*old_value
    f->output = (f->alpha)*new_value + (1 - f->alpha)*f->output;
    return (f->output);
}

/* Returns the current filter data, without modifying it */
float LPF_get_current_output(LowPassFilter* f){
    return (f->output);
}

