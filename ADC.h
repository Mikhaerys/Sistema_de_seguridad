/* 
 * File:   ADC.h
 * Author: Mikhaerys
 *
 * Created on October 20, 2023, 2:48 PM
 */

#ifndef ADC_H
#define	ADC_H

#ifdef	__cplusplus
extern "C" {
#endif

#define beta 4090
#define resistance 10
    
void adc_begin(void);
int adc_conversion(int channel);
int convertir_temperature(int temperature);


#ifdef	__cplusplus
}
#endif

#endif	/* ADC_H */

