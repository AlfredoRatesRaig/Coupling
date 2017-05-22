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

float Output;
int PID_Out_Min = 5;
int PID_Out_Max = 10;
int Setpoint = 100;
char Input = 0;

void main(void) {
    TRISB = 0xFF; //está bien?
    Input = PORTB; //está bien?
    PidType PID;
    PID_init(&PID, 1, 0.05, 0.25, PID_Direction_Direct);
    PID_SetMode(&PID, PID_Mode_Automatic);
    PID_SetOutputLimits(&PID, PID_Out_Min, PID_Out_Max);
    PID.mySetpoint = Setpoint;    
    PID.myInput = Input;
    PID_Compute(&PID);    
    Output = PID.myOutput;
    PORTD = Input; //está bien?
    return;
}
