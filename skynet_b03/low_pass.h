#ifndef LOW_PASS_H
#define LOW_PASS_H
typedef struct lpf{
    float output;
    float alpha;
} LowPassFilter;

void LPF_filter_init(LowPassFilter* f, float o, float a);
float LPF_update_filter(LowPassFilter* f, float new_value);
float LPF_get_current_output(LowPassFilter* f);
#endif
