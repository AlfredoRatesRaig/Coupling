/*
 * File:   newmain.c
 * Author: Alfredo Rates
 *
 * Created on March 13, 2017, 4:01 PM
 */


/*
 POR HACER:
 * Revisar código
 * Config y clock!!
 * ver como setear los puertos
 * Revisar como mandar 1s y 0s por los puertos
 * revisar como mandar un byte y recibir un byte por los puertos
 * revisar si se puede hacer todo el trabajo con bytes
 * Timer 
 * revisar main de asembler y tirarlo para aca
 * hacer las funciones para: encontrar setpoint, comunicacion para graficar*, forma de setear las constantes!
 */

/*
 FUNCIONAMIENTO DEL MAIN:
 * Abro el Mux
 * Seteo Mux en 0 (o 1,2,3)
 * Leo PUERTO B y guardo en A (o B,C,D)
 * Repito (4 veces)
 * Cierro el mux
 * Y = (A+C)-(B+D)  -   X = (A+B) - (C+D)
 * Se llama a PID con Input Y (o X)
 * Se recibe el Output del PID
 * Se envía el Output por PUERTO D
 */

#include <p18f4550.h>
#include <xc.h>
#include "pid.h"
#include "Config.h"
#define _XTAL_FREQ 32000000
float Output;
int PID_Out_Min = 0;
int PID_Out_Max = 10;
int Setpoint = 0;
float Input = 0;
float A;
float B;
float C;
float D;


void main(void) {
    TRISB = 0xFF; //está bien?
    TRISD = 0x00;
    TRISC = 0b01000000;
    TMR1CS = 0;
    TMR3CS = 0;
    PORTCbits.RC0 = 0;                        //Condiciones Iniciales
    PORTCbits.RC1 = 0;
    PORTCbits.RC2 = 1;
    PORTCbits.RC7 = 0;
    while(true){
        PORTCbits.RC0 = 0;                    //Mux en 00
        PORTCbits.RC1 = 0;
        
        PORTCbits.RC2 = 1;                    //Subo RD
        __delay_us(25);
        PORTCbits.RC2 = 0;                    //Subo RD
        A = PORTB;             //Lectura
        
        //__delay_ms(4000); 
        
        PORTCbits.RC0 = 1;                    //Mux en 01
        PORTCbits.RC1 = 0;
        
        PORTCbits.RC2 = 1;                    //Subo RD
        __delay_us(25);
        PORTCbits.RC2 = 0;                    //Subo RD
        B = PORTB;             //Lectura
        
        //__delay_ms(4000); 
        
        PORTCbits.RC0 = 0;                    //Mux en 10
        PORTCbits.RC1 = 1;
        
        PORTCbits.RC2 = 1;                    //Subo RD
        __delay_us(25);
        PORTCbits.RC2 = 0;                    //Subo RD
        C = PORTB;             //Lectura
        
        //__delay_ms(4000); 
        
        PORTCbits.RC0 = 1;                    //Mux en 11
        PORTCbits.RC1 = 1;
        
        PORTCbits.RC2 = 1;                    //Subo RD
        __delay_us(25);
        PORTCbits.RC2 = 0;                    //Subo RD
        D = PORTB;             //Lectura
        
        //__delay_ms(4000); 
        PORTCbits.RC7 = 0;
        //Input = (A+B)-(C+D); //está bien?
        Input = A;
        PidType PID;
        PID_init(&PID, 1, 0, 0, PID_Direction_Reverse);
        PID_SetMode(&PID, PID_Mode_Automatic);
        //PID_SetOutputLimits(&PID, PID_Out_Min, PID_Out_Max);
        PID.mySetpoint = Setpoint;    
        PID.myInput = Input;
        PID_Compute(&PID);    
        Output = PID.myOutput;
        //PORTD = Output; //está bien?
        PORTD = Input;
    }
    return;
}
