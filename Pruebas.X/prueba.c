/*
 * File:   prueba.c
 * Author: Astrolab
 *
 * Created on 29 de mayo de 2017, 10:28
 */

#include<p18f4550.h>                //Librerías
#include <xc.h>
#include "CONFIG.h"
#define _XTAL_FREQ 32000000         //Cristal: 20MHz ; Clock: 32MHz

float Input1;               //Declaración de variables
float Input2;
float Input3;
float Input4;

int main() 
{ 
    TRISB = 0xFF;                   //Declaración de puertos
    TRISD = 0x00;
    TRISC = 0b01000000;
    TMR1CS = 0;
    TMR3CS = 0;
    PORTCbits.RC0 = 0;                        //Condiciones Iniciales
    PORTCbits.RC1 = 0;
    PORTCbits.RC2 = 1;
    PORTCbits.RC7 = 0;
    //Input1 = 0xFF;
    //Input2 = 0x00;
    while(1) 
    {
        PORTCbits.RC0 = 0;                    //Mux en 00
        PORTCbits.RC1 = 0;
        PORTCbits.RC7 = 0;                    //Salida +
        
        PORTCbits.RC2 = 1;                    //Subo RD
        __delay_us(5);
        PORTCbits.RC2 = 0;
        while(PORTCbits.RC6 == 1);
        
        Input1 = PORTB;             //Lectura y escritura
        PORTD = Input1;
        
        //PORTCbits.RC7 = 0;        // lo cambie de lugar
        
        __delay_ms(4);           //Delay para prueba sin osciloscopio
        
        PORTCbits.RC0 = 1;                    //Mux en 01
        PORTCbits.RC1 = 0;
        PORTCbits.RC7 = 1;                    //Salida -
        
        PORTCbits.RC2 = 1;                    //Subo RD
        __delay_us(5);
        PORTCbits.RC2 = 0;
        while(PORTCbits.RC6 == 1);
        
        Input2 = PORTB;             //Lectura y escritura
        PORTD = Input2;
        
        //PORTCbits.RC7 = 1;        // lo cambie de lugar
        
        __delay_ms(4);           //Delay para prueba sin osciloscopio
        
        PORTCbits.RC0 = 0;                    //Mux en 10
        PORTCbits.RC1 = 1;
        PORTCbits.RC7 = 0;                    //Salida +
        
        PORTCbits.RC2 = 1;                    //Subo RD
        __delay_us(5);
        PORTCbits.RC2 = 0;
        while(PORTCbits.RC6 == 1);
        
        Input3 = PORTB;             //Lectura y escritura
        PORTD = Input3;
        
        //PORTCbits.RC7 = 0;        // lo cambie de lugar
        
        __delay_ms(4);           //Delay para prueba sin osciloscopio
        
        
        PORTCbits.RC0 = 1;                    //Mux en 11
        PORTCbits.RC1 = 1;
        PORTCbits.RC7 = 1;                    //Salida -
        
        PORTCbits.RC2 = 1;                    //Subo RD
        __delay_us(5);
        PORTCbits.RC2 = 0;
        while(PORTCbits.RC6 == 1);
     
        Input4 = PORTB;             //Lectura y escritura
        PORTD = Input4;
        
        //PORTCbits.RC7 = 1;        // lo cambie de lugar
        
        __delay_ms(4);           //Delay para prueba sin osciloscopio

    }
return 0;
}
    