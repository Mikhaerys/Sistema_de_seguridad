#include <xc.h>
#include "SENSOR.h"

unsigned char monitoring_sensor(void){
    unsigned char alarm_activ=0;
    if(SENSOR_IR == 0){
        alarm_activ=1;
    }
    if(SENSOR_HALL == 1){
        alarm_activ=1;
    }
    if(SENSOR_METAL == 1){
        alarm_activ=1;
    }
    return alarm_activ;
}

