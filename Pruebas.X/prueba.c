/*
 * File:   prueba.c
 * Author: Astrolab
 *
 * Created on 29 de mayo de 2017, 10:28
 */


#include<p18f4550.h>
#include <xc.h>
#include "CONFIG.h"
#define _XTAL_FREQ 32000000
float Input;
int main() 
{ 
    TRISD3=0;
    //TRISB = 0xFF; //está bien?
    //TRISD3 = 0;
    //TRISD2 = 1;
    while(1) 
    {
    RD3=1;                     //make RD3 pin High to glow LED 
    //for (int i=0; i<=400; i++) // 2.5 second delay
    __delay_ms(5000);           
    
    RD3=0;                     // make RD3 pin Low to Off LED 
    //for (int i=0; i<=400; i++)  // 1 second delay
    __delay_ms(5000);
    //Input = RD2; //está bien?
    //Input = Input*3;
    //Input = Input/3;
    //RD3=Input;
    } 
return 0;
}
