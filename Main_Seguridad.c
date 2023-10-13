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

//Fila salida del micro entrada del teclado
//Columna entrada del micro salida del teclado

enum State{
    Seguridad,
    Monitoreo,
    Bloqueado,
    Alarma
};

enum State Estado = Seguridad;

const char password[5] ={'1','2','3','4',0};
char pass_user[5];
unsigned char idx = 0;

unsigned char var_sensor_ir = 0;
unsigned char var_sensor_hall = 0;
unsigned char var_sensor_metal = 0;
unsigned char intentos = 3;

char buffer [17];

void function_seguridad(void);
void function_monitoreo(void);
void function_Bloqueado(void);
void function_alarma(void);


void main() {
    
    TRISE = 0;
    PORTE = 0x00;
    
    //Configura Fosc = 8Mhz interno, Fuente de Fosc del sistema = interno
    OSCCON = 0x71; //Configura oscilador interno (FOSC = 8Mhz)
     
    LCD_Init(); //Inicializa el LCD
    keypad_init(); //Inicializa el keypad
    
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
        else if(Estado == Alarma){
            function_alarma();
        }

    }
    
}

void function_monitoreo(void){
    if(monitoring_sensor() == 1){
        Estado = Alarma;
        int n = sprintf(buffer, "ir=%d,hl=%d,mt=%d", var_sensor_ir,var_sensor_hall,var_sensor_metal);
        LCD_String_xy(0,0,buffer);
        __delay_ms(2000);     
    }
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
    intentos = 3;
    Estado = Seguridad;
}

void function_alarma(void){
    LCD_Clear();
    LCD_String_xy(0,0,"SE TE ESTAN");
    LCD_String_xy(0,1,"METIENDO");
    for(int i = 0; i == 5; i++){
        PORTE = 0x04;
        __delay_ms(1000);
        PORTE = 0x00;
        __delay_ms(1000);
    }
    LCD_Clear();
    Estado = Monitoreo;
}