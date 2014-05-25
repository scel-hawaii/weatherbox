// Power.h

#ifndef POWERCONTROL_H
#define POWERCONTROL_H
#define __ACTIVE 1
#define __POWER_SAVE 0

#define __ON 1 
#define __OFF 0

/*
struct P_STATE{
    int xbee;
    int sensor_array;
};

P_STATE power_state;

// Sets the overall state of the system
PowerControl_setSystemState(int state){

}

void pstate_system(int state){
    if(state == __ACTIVE){
        pstate_xbee(__ON);
        pstate_sensor_array(__ON);
    }
    else if(state == __POWER_SAVE){
        pstate_xbee(__OFF);
        pstate_sensor_array(__OFF);
    }

}
// Switches the sleep states for the xbee
void pstate_xbee(int state){
    power_state.xbee = state; 
    sync_pstate();
}

// Switches the power state for the sensor array using the power switch
void pstate_sensor_array(int state){
    power_state.sensor_array = state; 
    sync_pstate();
}

// Sync the power states (called at the end of every pstate function)
void sync_pstate(void){
    digitalWrite(_PIN_XBEE_SLEEP, !power_state.xbee);
    digitalWrite(_PIN_PSWITCH, power_state.sensor_array);
}

*/
#endif
