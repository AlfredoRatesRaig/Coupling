/*******************************************************************************
Microchip's products.  Microchip and its licensors retain all ownership and
intellectual property rights in the accompanying software and in all
derivatives here to.

This software and any accompanying information is for suggestion only. It
does not modify Microchip's standard warranty for its products. You agree
that you are solely responsible for testing the software and determining its
suitability. Microchip has no obligation to modify, test, certify, or
support the software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER
EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR
PURPOSE APPLY TO THIS SOFTWARE, ITS INTERACTION WITH MICROCHIP?S PRODUCTS,
COMBINATION WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

IN NO EVENT, WILL MICROCHIP BE LIABLE, WHETHER IN CONTRACT, WARRANTY, TORT
(INCLUDING NEGLIGENCE OR BREACH OF STATUTORY DUTY), STRICT LIABILITY,
INDEMNITY, CONTRIBUTION, OR OTHERWISE, FOR ANY INDIRECT, SPECIAL, PUNITIVE,
EXEMPLARY, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, FOR COST OR EXPENSE OF
ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWSOEVER CAUSED, EVEN IF
MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.
TO THE FULLEST EXTENT ALLOWABLE BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL
CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF
FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF
THESE TERMS.
*******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include "../h/main.h"

/******************************************************************************/
/* Configuration Bits                                                         */
/******************************************************************************/
#pragma config FNOSC = FRC              // Oscillator Source Selection (Internal Fast RC (FRC))
#pragma config FCKSM = CSECMD           // Clock switching is enabled,Fail-safe Clock Monitor is disabled
#pragma config OSCIOFNC = ON            // OSC2 Pin Function bit (OSC2 is clock output)
#pragma config WDTEN = OFF              // Watchdog Timer Enable bits (WDT and SWDTEN disabled)
#pragma config ICS = PGD3               // ICD Communication Channel Select bits (Communicate on PGEC3 and PGED3)
#pragma config JTAGEN = OFF             // JTAG Enable bit (JTAG is disabled)
#pragma config PWMLOCK = OFF             // PWMx Lock Enable bit (Certain PWM registers may only be written after key sequency)

///* Interrupt Priority Level For Alternate Working Registers */
//#pragma config CTXT1 = IPL7              // Specifies Interrupt Priority Level (IPL) Associated to Alternate Working Register 1 bits 

/******************************************************************************/
/* Variable Declaration                                                */
/******************************************************************************/
volatile int16_t varCount;
int16_t varSSDelay,varSSDelay1;
int16_t varSelfTest,varTransientMode,varSwitch=1;
uint16_t varCurrentRef,varVoltageRefSS,varOpenLoopVoltage;
int32_t varProduct;

//MIAS

int Encoder_1a = 0;
int Encoder_2a = 0;
int Encoder_3a = 0;
int Encoder_4a = 0;
int Encoder_1b = 0;
int Encoder_2b = 0;
int Encoder_3b = 0;
int Encoder_4b = 0;

int16_t A11 = 0;
int16_t A21 = 0;
int16_t A12 = 0;
int16_t A22 = 0;

int16_t QPD_1 = 0;
int16_t QPD_2 = 0;
int16_t QPD_3 = 0;
int16_t QPD_4 = 0;

int16_t X = 0;
int16_t Y = 0;
int16_t Ux = 0;
int16_t Uy = 0;

/******************************************************************************/
/* FUNCTIONS                                                              */
/******************************************************************************/

void Controller(int16_t a11, int16_t a12, int16_t a21, int16_t a22){
    PORTAbits.RA1 = 1;
    while(1){
        QPD_1 = PORTDbits.RD1;
        QPD_2 = PORTDbits.RD2;
        QPD_3 = PORTDbits.RD3;
        QPD_4 = PORTDbits.RD4;

        X = QPD_1 + QPD_4 - QPD_2 - QPD_3;
        Y = QPD_1 + QPD_2 - QPD_3 - QPD_4;

        Ux = a11*X + a12*Y;
        Uy = a21*X + a22*Y;

        PORTDbits.RD5 = Ux;
        PORTDbits.RD6 = Uy;
    }
    PORTAbits.RA1 = 0;
    //INTERRUPT DEL BOTON ME HACE UN KILL DE ESTA FUNCION Y VUELVO AL MENU
    
}

void Encoders(int n, int a_old, int b_old, int a_new, int b_new){
    /*
   State | A B | direction
   ------+-----+----------
       1 | 1 0 | ^ forward (A leads B)
       2 | 1 1 | ^
       3 | 0 1 | ^
       4 | 0 0 | .
       1 | 1 0 | v
       2 | 1 1 | v
       3 | 0 1 | v reverse (B leads A)
*/
    int i = 0;
    
    if((a_old==0 && b_old==0 && a_new==0 && b_new==1) || (a_old==0 && b_old==1 && a_new==1 && b_new==1) || (a_old==1 && b_old==1 && a_new==1 && b_new==0) || (a_old==1 && b_old==0 && a_new==0 && b_new==0)){
        i = 1;
    }
    else if((a_old==0 && b_old==0 && a_new==1 && b_new==0) || (a_old==1 && b_old==0 && a_new==1 && b_new==1) || (a_old==1 && b_old==1 && a_new==0 && b_new==1) || (a_old==0 && b_old==1 && a_new==0 && b_new==0)){
        i = -1;
    }
    
    if(n==1){
        A11 = A11+i;
    }
    else if(n==2){
        A12 = A12+i;
    }
    else if(n==3){
        A21 = A21+i;
    }
    else if(n==4){
        A22 = A22+i;
    }
    // Actualizamos LCD
    //OJO OVERFLOW Y UNDERFLOW
    
}

void Anti_Rebotes(void){
    //pa evitar que se apriete el boton muchas veces
    while(PORTCbits.RC9){
    }
}

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/
int16_t main(void)
{
          
    /******************************************************************************/
    /* Initialization Routines                                                         */
    /******************************************************************************/    
    /* Configure the oscillator for the device */
    initClock();
    
    /* Initialize IO ports and peripherals */
    initIOPorts();          
    initPWM();
    initADC();
    initTimer();
    initCMP();
        
    /******************************************************************************/
    /* User Program                                                               */
    /******************************************************************************/

    //Prender LCD
    
    PORTAbits.RA1 = 0;               //EJEMPLO PIN DE LA LED!
    
    Encoder_1a = PORTCbits.RC1;      //EJEMPLO, PIN DEL ENCODER 1
    Encoder_2a = PORTCbits.RC2;      //EJEMPLO, PIN DEL ENCODER 2
    Encoder_3a = PORTCbits.RC3;      //EJEMPLO, PIN DEL ENCODER 3
    Encoder_4a = PORTCbits.RC4;      //EJEMPLO, PIN DEL ENCODER 4
    Encoder_1b = PORTCbits.RC5;      //EJEMPLO, PIN DEL ENCODER 1
    Encoder_2b = PORTCbits.RC6;      //EJEMPLO, PIN DEL ENCODER 2
    Encoder_3b = PORTCbits.RC7;      //EJEMPLO, PIN DEL ENCODER 3
    Encoder_4b = PORTCbits.RC8;      //EJEMPLO, PIN DEL ENCODER 4
    
    while(1){
        
        if(PORTCbits.RC9){      //EJEMPLO, PIN DEL BOTON
           Anti_Rebotes();
           Controller(A11, A12, A21, A22);
           Anti_Rebotes();
           Encoder_1a = PORTCbits.RC1;      //EJEMPLO, PIN DEL ENCODER 1
           Encoder_2a = PORTCbits.RC2;      //EJEMPLO, PIN DEL ENCODER 2
           Encoder_3a = PORTCbits.RC3;      //EJEMPLO, PIN DEL ENCODER 3
           Encoder_4a = PORTCbits.RC4;      //EJEMPLO, PIN DEL ENCODER 4
           Encoder_1b = PORTCbits.RC5;      //EJEMPLO, PIN DEL ENCODER 1
           Encoder_2b = PORTCbits.RC6;      //EJEMPLO, PIN DEL ENCODER 2
           Encoder_3b = PORTCbits.RC7;      //EJEMPLO, PIN DEL ENCODER 3
           Encoder_4b = PORTCbits.RC8;      //EJEMPLO, PIN DEL ENCODER 4
        }
        
        if(PORTCbits.RC1 != Encoder_1a || PORTCbits.RC5 != Encoder_1b){
            Encoders(1, Encoder_1a, Encoder_1b, PORTCbits.RC1, PORTCbits.RC5);
            Encoder_1a = PORTCbits.RC1;
            Encoder_1b = PORTCbits.RC5;
        }
        if(PORTCbits.RC2 != Encoder_2a || PORTCbits.RC6 != Encoder_2b){
            Encoders(2, Encoder_2a, Encoder_2b, PORTCbits.RC2, PORTCbits.RC6);
            Encoder_2a = PORTCbits.RC1;
            Encoder_2b = PORTCbits.RC5;
        }
        if(PORTCbits.RC3 != Encoder_3a || PORTCbits.RC7 != Encoder_3b){
            Encoders(3, Encoder_3a, Encoder_3b, PORTCbits.RC3, PORTCbits.RC7);
            Encoder_3a = PORTCbits.RC1;
            Encoder_3b = PORTCbits.RC5;
        }
        if(PORTCbits.RC4 != Encoder_4a || PORTCbits.RC8 != Encoder_4b){
            Encoders(4, Encoder_4a, Encoder_4b, PORTCbits.RC4, PORTCbits.RC8);
            Encoder_4a = PORTCbits.RC1;
            Encoder_4b = PORTCbits.RC5;
        }
    }
}