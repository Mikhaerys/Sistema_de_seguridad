#include    <xc.h>
#include    <pic16f887.h>
#include    <math.h>
#include    "CONFIG.h"
#include    "ADC.h"


void adc_begin(void) {
    ADCON1bits.ADFM = 0; //  Justificacion Izquierda (modo-8bits).
    ADCON1bits.VCFG0 = 0; //  Selecciona Voltajes de Referencia (5v-0v).
    ADCON1bits.VCFG1 = 0; //  Selecciona Voltajes de Referencia (5v-0v). 
    ADCON0bits.ADCS = 0b01; //  Tiempo de Conversi?n Fosc/8.
    
}

int adc_conversion(int channel){
    
    ADCON0bits.CHS = (0x0F & channel);
    ADCON0bits.ADON = 1; //  Habilita el Modulo AD.
    __delay_us(30);
    ADCON0bits.GO = 1; //  Inicia la Conversion AD.
    while (ADCON0bits.GO); //  Espera a que termine la conversion AD.
    ADCON0bits.ADON = 0; //  Habilita el Modulo AD.
    return ((ADRESH << 8) | ADRESL);  
}

int convertir_temperature(int temperature){
      
    long a = 1023 - temperature;
    float tempC = (float) (beta / (log((1025.0 * 10 / a - 10) / 10) + beta / 298.0) - 273);
    float tempF = (float) (tempC + 273.15);
    return (int)tempC;
}