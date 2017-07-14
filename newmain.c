/*
 * File:   newmain.c
 * Author: Alfredo Rates
 *
 * Created on March 13, 2017, 4:01 PM
 */


/*
 POR HACER:
 * Revisar c�digo
 * Config y clock!!
 * revisar si se puede hacer todo el trabajo con bytes
 * Timer 
 * hacer las funciones para: comunicacion para graficar*, forma de setear las constantes!
 */

#include <p18f4550.h>
#include <xc.h>
#include "Config.h"
#define _XTAL_FREQ 32000000
int Output;
//int PID_Out_Min = -10000;
//int PID_Out_Max = 10000;
int Setpoint = 0;
int Input = 0;
int A;
int B;
int C;
int D;
int sign;
unsigned char output;

void main(void) {
    TRISB = 0xFF; //est� bien?
    TRISD = 0x00;
    TRISC = 0b01000000;
    TMR1CS = 0;
    TMR3CS = 0;
    PORTCbits.RC0 = 0;                        //Condiciones Iniciales
    PORTCbits.RC1 = 0;
    PORTCbits.RC2 = 1;
    PORTCbits.RC7 = 0;
    int count = 0;
    int err;
    while (count<1000){
        count = count+1;
        PORTCbits.RC0 = 0;                    //Mux en 00
        PORTCbits.RC1 = 0;
        
        PORTCbits.RC2 = 1;                    //Subo RD
        __delay_us(5);
        PORTCbits.RC2 = 0;                    //Subo RD
        //__delay_us(50);
        while(PORTCbits.RC6 == 1);
        A = PORTB;             //Lectura
        
        //__delay_us(40); 
        
        PORTCbits.RC0 = 1;                    //Mux en 01
        PORTCbits.RC1 = 0;
        
        PORTCbits.RC2 = 1;                    //Subo RD
        __delay_us(5);
        PORTCbits.RC2 = 0;                    //Subo RD
        //__delay_us(50);
        while(PORTCbits.RC6 == 1);
        B = PORTB;             //Lectura
        
        //__delay_us(40); 
        
        PORTCbits.RC0 = 0;                    //Mux en 10
        PORTCbits.RC1 = 1;
        
        PORTCbits.RC2 = 1;                    //Subo RD
        __delay_us(5);
        PORTCbits.RC2 = 0;                    //Subo RD
        //__delay_us(50);
        while(PORTCbits.RC6 == 1);  
        C = PORTB;             //Lectura
        
        //__delay_us(40);  
        
        PORTCbits.RC0 = 1;                    //Mux en 11
        PORTCbits.RC1 = 1;
        
        PORTCbits.RC2 = 1;                    //Subo RD
        __delay_us(5);
        PORTCbits.RC2 = 0;                    //Subo RD
        //__delay_us(50);
        while(PORTCbits.RC6 == 1);
        D = PORTB;             //Lectura
        
        //__delay_us(40); 
        err = (A+B)-(C+D);
        Setpoint = Setpoint+err;
    }
    Setpoint = Setpoint/1000;
    
    while(1){
        
    //Lectura de los 4 photodiodes
        
        PORTCbits.RC0 = 0;                    //Mux en 00
        PORTCbits.RC1 = 0;
        
        PORTCbits.RC2 = 1;                    //Subo RD
        __delay_us(5);
        PORTCbits.RC2 = 0;                    //Subo RD
        //__delay_us(50);
        while(PORTCbits.RC6 == 1);
        A = PORTB;             //Lectura
        
        //__delay_us(40); 
        
        PORTCbits.RC0 = 1;                    //Mux en 01
        PORTCbits.RC1 = 0;
        
        PORTCbits.RC2 = 1;                    //Subo RD
        __delay_us(5);
        PORTCbits.RC2 = 0;                    //Subo RD
        //__delay_us(50);
        while(PORTCbits.RC6 == 1);
        B = PORTB;             //Lectura
        
        //__delay_us(40); 
        
        PORTCbits.RC0 = 0;                    //Mux en 10
        PORTCbits.RC1 = 1;
        
        PORTCbits.RC2 = 1;                    //Subo RD
        __delay_us(5);
        PORTCbits.RC2 = 0;                    //Subo RD
        //__delay_us(50);
        while(PORTCbits.RC6 == 1);  
        C = PORTB;             //Lectura
        
        //__delay_us(40);  
        
        PORTCbits.RC0 = 1;                    //Mux en 11
        PORTCbits.RC1 = 1;
        
        PORTCbits.RC2 = 1;                    //Subo RD
        __delay_us(5);
        PORTCbits.RC2 = 0;                    //Subo RD
        //__delay_us(50);
        while(PORTCbits.RC6 == 1);
        D = PORTB;             //Lectura
        
        //__delay_us(40); 
        
    //Operaci�n matem�tica de entradas
        
        Input = (A+B)-(C+D);
        Output = Input-Setpoint;
        //PidType PID;
        //PID_init(&PID, 1, 0, 0, PID_Direction_Reverse);
        //PID_SetMode(&PID, PID_Mode_Automatic);
        //PID_SetOutputLimits(&PID, PID_Out_Min, PID_Out_Max);
        //PID.mySetpoint = Setpoint;    
        //PID.myInput = Input;
        //PID_Compute(&PID);  
        //Output = PID.myOutput;
        
        if(Output>0){
            Output = Output;
            PORTCbits.RC7 = 0;
        }
        else{
            Output = -Output;
            PORTCbits.RC7 = 1;
        }
        output = Output;
        PORTD = output;
    }
    return;
}
