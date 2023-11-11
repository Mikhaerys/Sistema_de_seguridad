#include <xc.h>
#include "SENSOR.h"

/**
 * @brief Monitorea el estado de los sensores de seguridad.
 *
 * Esta funci�n monitorea el estado de los sensores de seguridad, incluyendo el sensor de infrarrojos (SENSOR_IR),
 * el sensor Hall (SENSOR_HALL) y el sensor de metal (SENSOR_METAL). Si alguno de estos sensores detecta una intrusi�n,
 * la funci�n devuelve un valor de activaci�n de alarma (1); de lo contrario, devuelve un valor de desactivaci�n de alarma (0).
 *
 * @note Esta funci�n utiliza las constantes SENSOR_IR, SENSOR_HALL y SENSOR_METAL.
 *
 * @return Valor de activaci�n de alarma (1) si se detecta una intrusi�n, de lo contrario, valor de desactivaci�n de alarma (0).
 */
unsigned char monitoring_sensor(void){
    unsigned char alarm_activ=0;
    
    // Verifica si el sensor de infrarrojos detecta una intrusi�n
    if(SENSOR_IR == 0){
        alarm_activ=1;
    }
    // Verifica si el sensor Hall detecta una intrusi�n
    if(SENSOR_HALL == 1){
        alarm_activ=1;
    }
    // Verifica si el sensor de metal detecta una intrusi�n
    if(SENSOR_METAL == 1){
        alarm_activ=1;
    }
    return alarm_activ;
}

