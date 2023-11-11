#include    <xc.h>
#include    <pic16f887.h>
#include    <math.h>
#include    "CONFIG.h"
#include    "ADC.h"

/**
 * @brief Inicializa la configuraci�n del m�dulo ADC.
 *
 * Esta funci�n inicializa la configuraci�n del m�dulo ADC. Establece la justificaci�n a la izquierda (modo de 8 bits),
 * selecciona los voltajes de referencia (de 0 a 5 voltios), y configura el tiempo de conversi�n a Fosc/8.
 *
 * @note Esta funci�n utiliza las constantes ADCON1bits.ADFM, ADCON1bits.VCFG0, ADCON1bits.VCFG1 y ADCON0bits.ADCS, que deben estar definidas previamente en el c�digo.
 *
 */
void adc_begin(void) {
    ADCON1bits.ADFM = 1; //  Justificacion Izquierda (modo-8bits).
    ADCON1bits.VCFG0 = 0; //  Selecciona Voltajes de Referencia (5v-0v).
    ADCON1bits.VCFG1 = 0; //  Selecciona Voltajes de Referencia (5v-0v). 
    ADCON0bits.ADCS = 0b01; //  Tiempo de Conversion Fosc/8.
    
}

/**
 * @brief Realiza una conversi�n anal�gica a digital (ADC).
 *
 * Esta funci�n realiza una conversi�n anal�gica a digital (ADC) para el canal especificado. Configura el canal de
 * conversi�n, inicia la conversi�n ADC y espera a que termine. Luego, deshabilita el m�dulo ADC y devuelve el resultado
 * de la conversi�n como un entero de 16 bits.
 *
 * @param channel N�mero de canal para la conversi�n ADC.
 * @return Resultado de la conversi�n ADC como un entero de 16 bits.
 *
 * @note Esta funci�n utiliza las constantes ADCON0bits.CHS, ADCON0bits.ADON, ADCON0bits.GO, ADRESH y ADRESL, que deben estar definidas previamente en el c�digo.
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
 * Esta funci�n realiza la conversi�n de una lectura de temperatura en grados Celsius. Utiliza la f�rmula de Steinhart-Hart
 * para convertir la lectura del sensor de temperatura a grados Celsius. La funci�n devuelve el valor de temperatura en grados
 * Celsius como un entero.
 *
 * @param temperature Valor de la lectura del sensor de temperatura.
 * @return Valor de temperatura en grados Celsius como un entero.
 *
 * @note Esta funci�n utiliza la constante beta, que debe estar definida previamente en el c�digo.
 *
 */
int convertir_temperature(int temperature){
      
    long a = 1023 - temperature;
    float tempC = (float) (beta / (log((1025.0 * 10 / a - 10) / 10) + beta / 298.0) - 273);
    float tempF = (float) (tempC + 273.15);
    // Se convierte a grados Celsius y se devuelve como un entero
    return (int) (tempC);
}