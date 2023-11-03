/*!
\file   seguridad.c
\date   2023-10-06
\author Miguel Angel Maldonado Bautista, Catalina Torres Arenas <mmaldonado@unicauca.edu.co><catatorres@unicauca.edu.co>
\brief  Security House.

\par Copyright
Information contained herein is proprietary to and constitutes valuable
confidential trade secrets of unicauca, and
is subject to restrictions on use and disclosure.

\par
Copyright (c) unicauca 2022. All rights reserved.

\par
The copyright notices above do not evidence any actual or
intended publication of this material.
******************************************************************************
*/

#include <xc.h>
#include <pic16f887.h>  /*Header file PIC16f887 definitions*/
#include <stdio.h>
#include <string.h>
#include "CONFIG.h"
#include "LCD.h"
#include "KEYPAD.h"
#include "SENSOR.h"
#include "ADC.h"

//Fila salida del micro entrada del teclado
//Columna entrada del micro salida del teclado

enum State{
    Ambiental,
    Seguridad,
    Monitoreo,
    Bloqueado,
    Alarma_Seguridad,
    Alarma_Ambiental
};

enum State Estado = Seguridad;

const char password[5] = {'1','2','3','4',0};
char pass_user[5];
unsigned char idx = 0;

unsigned char var_sensor_ir = 0;
unsigned char var_sensor_hall = 0;
unsigned char var_sensor_metal = 0;
unsigned char intentos = 3;

char buffer [17];

int value_potenciometro;
int value_temperature;
int value_protocell;
int tempCelsius;

unsigned long  count_timer_lms = 0;
int init_time = 0;
int end_time = 0;
#define Pulse PORTB
#define INTERVAL_DELAY 5000

void Timer1_start();

void function_ambiental(void);
void function_seguridad(void);
void function_monitoreo(void);
void function_Bloqueado(void);
void function_alarma_seguridad(void);
void function_alarma_ambiental(void);


void main() {

    //Configura Fosc = 8Mhz interno, Fuente de Fosc del sistema = interno
    OSCCON = 0x71; //Configura oscilador interno (FOSC = 8Mhz)
    TRISD = 0x00;// salida puerto D
    TRISA0 = 1;
    ANSEL = 0x01; //  Configura el Puerto como Entrada Anal?gica.
    
    TRISE = 0;
    PORTE = 0x00;
     
    LCD_Init(); //Inicializa el LCD
    keypad_init(); //Inicializa el keypad
    adc_begin();
    
    TRISC = 0x07;
    
    var_sensor_ir = SENSOR_IR;
    var_sensor_hall = SENSOR_HALL;
    var_sensor_metal = SENSOR_METAL;

    while (1) {
        if(Estado == Seguridad){
            function_seguridad();
        }
        else if(Estado == Monitoreo){
            function_monitoreo();
        }
        else if(Estado == Bloqueado){
            function_Bloqueado();
        }
        else if(Estado == Alarma_Seguridad){
            function_alarma_seguridad();
        }
        else if(Estado == Ambiental){
            function_ambiental();
        }
        else if(Estado == Alarma_Ambiental){
            function_alarma_ambiental();
        }
        
    }
    
}

void function_ambiental(void){
    value_potenciometro = adc_conversion(0);
    value_temperature = adc_conversion(1);
    tempCelsius = convertir_temperature(value_temperature);
    value_protocell = adc_conversion(4);   
    
    int n = sprintf(buffer, "ir=%d,hl=%d,mt=%d", value_potenciometro ,tempCelsius ,value_protocell);
    LCD_String_xy(1,0,buffer);
    __delay_ms(3000); 
    LCD_Clear();
    
    if(tempCelsius > 30){
        Estado = Alarma_Ambiental;
    }
    else{
        Estado = Monitoreo;
    }
}

void function_monitoreo(void){
    
    
    if(monitoring_sensor() == 1){
        Estado = Alarma_Seguridad;
        int n = sprintf(buffer, "ir=%d,hl=%d,mt=%d", var_sensor_ir,var_sensor_hall,var_sensor_metal);
        LCD_String_xy(0,0,buffer);   
    }
    else{
        Estado = Ambiental;
    }
    __delay_ms(2000);
}

void function_seguridad(void){
        char key = '0';
        
        if (intentos != 0){
            
            LCD_String_xy(0,0,"ENTER PASSWORD:");
            LCD_Command(0xC0); 
            
            do{
                key = keypad_getkey();
                
                if(key != 0){
                    LCD_Char('*');/* display pressed key on LCD16x2 */
                    //LCD_Char(key);/* display pressed key on LCD16x2 */
                    pass_user[idx++] = key;
                }
                __delay_ms(100);
            }while(idx < 4);

            if(strncmp(pass_user,password,4)==0){
                LCD_Clear();
                LCD_String_xy(0,0,"Clave Correcta");
                PORTE = 0x02;
                intentos = 3;
                Estado = Monitoreo;
            }
            
            else{
                LCD_Clear();
                LCD_String_xy(0,0,"Clave Incorrecta");
                intentos--;
                PORTE = 0x01;
            }
        }
        else{
            Estado = Bloqueado;
        }
        
        __delay_ms(2000);
        LCD_Clear();
        idx = 0;
        PORTE = 0x00;
}

void function_Bloqueado (void){
    LCD_Clear();
    LCD_String_xy(0,0,"Sistema Bloqueado");
    PORTE = 0x04;
    __delay_ms(10000);
    PORTE = 0x00;
    LCD_Clear();
    intentos = 3;
    Estado = Seguridad;
}

void function_alarma_seguridad(void){
    LCD_Clear();
    LCD_String_xy(0,0,"ALARMA SECURITY");
    
    for(int i = 0; i < 5; i++){
        PORTE = 0x04;
        __delay_ms(500);
        PORTE = 0x00;
        __delay_ms(500);
    }
    LCD_Clear();
    Estado = Monitoreo;
}

void function_alarma_ambiental(void){   
    LCD_Clear();
    LCD_String_xy(0,0,"ALARMA AMBIENTE");
    for(int i = 0; i < 5; i++){
        PORTE = 0x04;
        __delay_ms(1000);
        PORTE = 0x00;
        __delay_ms(1000);
    }
    LCD_Clear();
    Estado = Ambiental;
}

void __interrupt() Timer_ISR(){
    count_timer_lms++;
    TMR1 = 0xFC16;
    Pulse = ~Pulse;
    PIR1bits.TMR1IF = 0;
}

void Timer1_start(){
    
}

