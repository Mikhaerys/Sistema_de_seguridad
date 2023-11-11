#include    <xc.h>
#include    <pic16f887.h>
#include    <math.h>
#include    "CONFIG.h"
#include    "ADC.h"

/**
 * @brief Inicializa la configuración del módulo ADC.
 *
 * Esta función inicializa la configuración del módulo ADC. Establece la justificación a la izquierda (modo de 8 bits),
 * selecciona los voltajes de referencia (de 0 a 5 voltios), y configura el tiempo de conversión a Fosc/8.
 *
 * @note Esta función utiliza las constantes ADCON1bits.ADFM, ADCON1bits.VCFG0, ADCON1bits.VCFG1 y ADCON0bits.ADCS, que deben estar definidas previamente en el código.
 *
 */
void adc_begin(void) {
    ADCON1bits.ADFM = 1; //  Justificacion Izquierda (modo-8bits).
    ADCON1bits.VCFG0 = 0; //  Selecciona Voltajes de Referencia (5v-0v).
    ADCON1bits.VCFG1 = 0; //  Selecciona Voltajes de Referencia (5v-0v). 
    ADCON0bits.ADCS = 0b01; //  Tiempo de Conversion Fosc/8.
    
}

/**
 * @brief Realiza una conversión analógica a digital (ADC).
 *
 * Esta función realiza una conversión analógica a digital (ADC) para el canal especificado. Configura el canal de
 * conversión, inicia la conversión ADC y espera a que termine. Luego, deshabilita el módulo ADC y devuelve el resultado
 * de la conversión como un entero de 16 bits.
 *
 * @param channel Número de canal para la conversión ADC.
 * @return Resultado de la conversión ADC como un entero de 16 bits.
 *
 * @note Esta función utiliza las constantes ADCON0bits.CHS, ADCON0bits.ADON, ADCON0bits.GO, ADRESH y ADRESL, que deben estar definidas previamente en el código.
 *
 */
int adc_conversion(int channel){
    
    ADCON0bits.CHS = (0x0F & channel);
    ADCON0bits.ADON = 1; //  Habilita el Modulo AD.
    __delay_us(30);
    ADCON0bits.GO = 1; //  Inicia la Conversion AD.
    while (ADCON0bits.GO); //  Espera a que termine la conversion AD.
    ADCON0bits.ADON = 0; //  Habilita el Modulo AD.
    return ((ADRESH << 8) | ADRESL);  
}

/**
 * @brief Convierte una lectura de temperatura en grados Celsius.
 *
 * Esta función realiza la conversión de una lectura de temperatura en grados Celsius. Utiliza la fórmula de Steinhart-Hart
 * para convertir la lectura del sensor de temperatura a grados Celsius. La función devuelve el valor de temperatura en grados
 * Celsius como un entero.
 *
 * @param temperature Valor de la lectura del sensor de temperatura.
 * @return Valor de temperatura en grados Celsius como un entero.
 *
 * @note Esta función utiliza la constante beta, que debe estar definida previamente en el código.
 *
 */
int convertir_temperature(int temperature){
      
    long a = 1023 - temperature;
    float tempC = (float) (beta / (log((1025.0 * 10 / a - 10) / 10) + beta / 298.0) - 273);
    float tempF = (float) (tempC + 273.15);
    // Se convierte a grados Celsius y se devuelve como un entero
    return (int) (tempC);
}