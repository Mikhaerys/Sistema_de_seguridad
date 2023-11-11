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

#define CHS_TEMPERATURE 1

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
#define INTERVAL_DELAY 3000

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

/**
 * @brief Maneja el estado ambiental y realiza la monitorización de sensores.
 *
 * Esta función gestiona el estado ambiental y realiza la monitorización de sensores. Adquiere lecturas de sensores
 * como el potenciómetro, la temperatura y la protocelda mediante conversiones ADC. Muestra la información en la pantalla
 * LCD y evalúa las condiciones ambientales. Si la temperatura supera los 30 grados Celsius, cambia al estado de Alarma
 * Ambiental; de lo contrario, regresa al estado de Monitoreo después de un intervalo de tiempo definido.
 *
 * @note Esta función utiliza las funciones `adc_conversion`, `convertir_temperature`, `LCD_Clear`, `LCD_String_xy`, y `count_timer_lms`.
 *
 * @see adc_conversion
 * @see convertir_temperature
 * @see LCD_Clear
 * @see LCD_String_xy
 * @see count_timer_lms
 * 
 * @param None
 * @return None
 */
void function_ambiental(void){
    // Adquiere lecturas de sensores
    value_potenciometro = adc_conversion(0);
    value_temperature = adc_conversion(CHS_TEMPERATURE);
    tempCelsius = convertir_temperature(value_temperature);
    value_protocell = adc_conversion(4);   
    
    // Muestra la información de los sensores en la pantalla LCD
    int n = sprintf(buffer, "T=%d,P=%d,PR=%d", tempCelsius, value_potenciometro ,value_protocell);
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

/**
 * @brief Monitorea los sensores de seguridad e información ambiental.
 *
 * Esta función realiza el monitoreo de los sensores de seguridad y ambientales. Si se detecta
 * una intrusión a través del sensor de seguridad, activa el estado de Alarma de Seguridad y muestra
 * la información relevante en la pantalla LCD. Si no se detecta intrusión, establece el estado como Ambiental.
 * Después de cada monitoreo, se realiza una pausa de 2000 milisegundos y se reinicia el temporizador.
 *
 * @note Esta función utiliza las funciones `monitoring_sensor`, `LCD_String_xy` y `count_timer_lms`.
 *
 * @see monitoring_sensor
 * @see LCD_String_xy
 * @see count_timer_lms
 * 
 * @param None
 * @return None
 */
void function_monitoreo(void){
    
    if(monitoring_sensor() == 1){
        Estado = Alarma_Seguridad;
        int n = sprintf(buffer, "ir=%d,hl=%d,mt=%d", var_sensor_ir,var_sensor_hall,var_sensor_metal);
        LCD_String_xy(0,0,buffer);   
    }
    else{
        LCD_Clear();
        LCD_String_xy(0,0,"No se detecta");
        Estado = Ambiental;
    }
    __delay_ms(2000);
}

/**
 * @brief Gestiona el proceso de seguridad y entrada de contraseña.
 *
 * Esta función maneja la entrada de la contraseña a través de un teclado y verifica su validez.
 * Si la contraseña ingresada es correcta, se muestra un mensaje en la pantalla LCD y se permite
 * el acceso al estado de Monitoreo. En caso de una contraseña incorrecta, se actualizan los intentos
 * y se indica en la pantalla LCD. Si se agotan los intentos, se bloquea el acceso al estado de Bloqueado.
 * Después de cada intento, hay una pausa de 2000 milisegundos antes de limpiar la pantalla LCD y reiniciar
 * las variables relacionadas con la entrada de la contraseña.
 *
 * @note Esta función utiliza las funciones `keypad_getkey`, `LCD_String_xy`, `LCD_Command`, y `LCD_Clear`.
 *
 * @see keypad_getkey
 * @see LCD_String_xy
 * @see LCD_Command
 * @see LCD_Clear
 * 
 * @param None
 * @return None
 * 
 */
void function_seguridad(void){
        char key = '0';
        
        // Verifica si hay intentos restantes
        if (intentos != 0){
            
            LCD_String_xy(0,0,"ENTER PASSWORD:");
            LCD_Command(0xC0); 
            
            do{
                key = keypad_getkey();
                
                if(key != 0){
                    LCD_Char('*'); /* Muestra el carácter '*' en la pantalla LCD */
                    pass_user[idx++] = key;
                }
                __delay_ms(100);
            }while(idx < 4);

            // Verifica si la contraseña ingresada es correcta
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
        
        // Pausa de 2000 milisegundos antes de limpiar la pantalla y reiniciar variables
        __delay_ms(2000);
        LCD_Clear();
        idx = 0;
        PORTE = 0x00;
}

/**
 * @brief Maneja el estado de Bloqueado del sistema.
 *
 * Esta función se encarga de gestionar el estado de Bloqueado del sistema. Muestra un mensaje en la pantalla LCD
 * indicando que el sistema está bloqueado, activa una señal en el puerto E y luego espera durante 10 segundos antes
 * de restablecer la configuración y volver al estado de Seguridad. Durante el estado Bloqueado, los intentos se restablecen
 * a su valor inicial.
 *
 * @note Esta función utiliza las funciones `LCD_Clear` y `LCD_String_xy`.
 *
 * @see LCD_Clear
 * @see LCD_String_xy
 * 
 * @param None
 * @return None
 */
void function_Bloqueado (void){
    // Limpia la pantalla LCD y muestra un mensaje de sistema bloqueado
    LCD_Clear();
    LCD_String_xy(0, 0, "Sistema Bloqueado");
    PORTE = 0x04;  // Activa una señal en el puerto E indicando estado bloqueado
    __delay_ms(10000);  // Espera durante 10 segundos

    // Restablece la configuración y vuelve al estado de Seguridad
    PORTE = 0x00;
    LCD_Clear();
    intentos = 3;  // Restablece los intentos a su valor inicial
    Estado = Seguridad;
}

/**
 * @brief Maneja el estado de activación de la alarma de seguridad.
 *
 * Esta función gestiona el estado de activación de la alarma de seguridad. Muestra un mensaje en la pantalla LCD
 * indicando que la alarma de seguridad está activada. Durante la activación, se realiza un ciclo de encendido y
 * apagado de la señal en el puerto E para simular una alarma visual. Después de cinco ciclos, la función limpia la
 * pantalla LCD y cambia al estado de Monitoreo.
 *
 * @note Esta función utiliza las funciones `LCD_Clear` y `LCD_String_xy`.
 *
 * @see LCD_Clear
 * @see LCD_String_xy
 * 
 * @param None
 * @return None
 */
void function_alarma_seguridad(void){
    // Limpia la pantalla LCD y muestra un mensaje de activación de la alarma de seguridad
    LCD_Clear();
    LCD_String_xy(0,0,"ALARMA SECURITY");
    
    // Realiza cinco ciclos de encendido y apagado de la señal en el puerto E
    for(int i = 0; i < 5; i++){
        PORTE = 0x04; 
        __delay_ms(500);
        PORTE = 0x00;
        __delay_ms(500);
    }
    // Limpia la pantalla LCD y cambia al estado de Monitoreo
    LCD_Clear();
    Estado = Monitoreo;
}

/**
 * @brief Maneja el estado de activación de la alarma ambiental.
 *
 * Esta función gestiona el estado de activación de la alarma ambiental. Muestra un mensaje en la pantalla LCD
 * indicando que la alarma ambiental está activada. Durante la activación, se realiza un ciclo de encendido y
 * apagado de la señal en el puerto E para simular una alarma visual. Después de cinco ciclos, la función limpia la
 * pantalla LCD y cambia al estado Ambiental.
 *
 * @note Esta función utiliza las funciones `LCD_Clear` y `LCD_String_xy`.
 *
 * @see LCD_Clear
 * @see LCD_String_xy
 * 
 * @param None
 * @return None
 */
void function_alarma_ambiental(void){   
    // Limpia la pantalla LCD y muestra un mensaje de activación de la alarma ambiental
    LCD_Clear();
    LCD_String_xy(0,0,"ALARMA AMBIENTE");
    
    // Realiza cinco ciclos de encendido y apagado de la señal en el puerto E
    for(int i = 0; i < 5; i++){
        PORTE = 0x04;
        __delay_ms(1000);
        PORTE = 0x00;
        __delay_ms(1000);
    }
    // Limpia la pantalla LCD y cambia al estado Ambiental
    LCD_Clear();
    Estado = Ambiental;
}

/**
 * @brief Rutina de servicio de interrupción para el Timer1.
 *
 * Esta función es la rutina de servicio de interrupción (ISR) para el Timer1. Incrementa una variable de cuenta (`count_timer_lms`)
 * que puede utilizarse para medir el tiempo transcurrido. Reinicia el contador Timer1 para generar una interrupción cada 1 ms. Además,
 * alterna el estado de la variable `Pulse` y limpia el flag de interrupción de Timer1 (`TMR1IF`).
 *
 * @note Esta función utiliza las constantes count_timer_lms, TMR1, Pulse y PIR1bits.TMR1IF, que deben estar definidas previamente en el código.
 *
 * @param None
 * @return None
 */
void __interrupt() Timer_ISR(){
    count_timer_lms++;
    TMR1 = 0xFC16;
    Pulse = ~Pulse;
    PIR1bits.TMR1IF = 0;
}

/**
 * @brief Inicia el temporizador Timer1 para generar interrupciones por desbordamiento.
 *
 * Esta función inicia el temporizador Timer1 para generar interrupciones por desbordamiento. Habilita las interrupciones
 * globales, las interrupciones periféricas y la interrupción de desbordamiento de Timer1. Configura el temporizador para
 * funcionar en modo de 16 bits sin preescala, utiliza el reloj interno y lo deja apagado inicialmente. Luego, carga el
 * valor inicial en el registro TMR1 para generar una interrupción cada 1 ms.
 *
 * @note Esta función utiliza las constantes GIE, PEIE, TMR1IE, TMR1IF, T1CON, TMR1 y TMR1ON, que deben estar definidas previamente en el código.
 *
 * @param None
 * @return None
 */
void Timer1_start(){
    GIE=1;              /* Enable Global Interrupt */
    PEIE=1;             /* Enable Peripheral Interrupt */
    TMR1IE=1;           /* Enable Timer1 Overflow Interrupt */
    TMR1IF=0;
    T1CON=0x00;         /* Enable 16-bit TMR1 Register,No pre-scale,use internal clock,Timer OFF */
    TMR1=0xFC16;        /* Load Count for generating delay of 1ms */
    TMR1ON=1;           /* Turn-On Timer1 */
}

