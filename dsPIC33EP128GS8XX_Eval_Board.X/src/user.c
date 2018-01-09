/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include <p33Exxxx.h> 
#include <stdint.h>          /* For uint16_t definition                       */
#include <stdio.h>           
#include "libpic30.h"
#include "../h/user.h"           /* variables/params used by user.c               */
#include "../h/init.h"

int16_t varDelay;
uint8_t varTemp;

/*******************************************************************************
Function: 	softStartRoutine
Description:	Soft Start Activities
*******************************************************************************/
void softStartRoutine(int16_t delay, int16_t refVoltage)
{
    varDelay = __builtin_muluu(TMR_uS_SCALER,delay); /* Multiplier */
    PR1=(varDelay >> 3);    /* TMR1SCALER = 1 */
    
    StartTimer1();             /* Start Timer1 */
    while(varCount < refVoltage)                                            
    {
        varVoltageRef = varCount;
    }
    StopTimer1();           /* Turnoff Timer1 */
    PR1 = TMR1PERIOD;       /* Revert back to 1ms */
}

/*******************************************************************************
Function: 	readR12
Description:	Reads Potentiometer R12 Voltage
*******************************************************************************/
uint16_t readR12(void)
{
    ADCON4Hbits.C2CHS = 0;       /* AN4 */
    ADCON3Lbits.CNVRTCH = 1;     /* Trigger ADC conversion */
    __delay_us(1);  
    return(ADCBUF2);
};

/*******************************************************************************
Function: 	readR20
Description:	Reads Potentiometer R20 Voltage
*******************************************************************************/

uint16_t readR20(void)
{
    ADCON4Hbits.C2CHS = 1;       /* AN11 */
    ADCON3Lbits.CNVRTCH = 1;     /* Trigger ADC conversion */
    __delay_us(1);  
    return(ADCBUF2);
};

/*******************************************************************************
Function: 	SwitchPress
Description:	Detects switch press condition
*******************************************************************************/
void SwitchDetect(void)
{
    if(!SW1)
    {
        StartTimer1();
        while(varCount <= DEBOUNCE_DELAY)
        {
        };
        varSwitch = 0;  /* Indicate Switch Press */
        
        while(!SW1)
        {
            if(varCount >= SELFTESTTIME)
                LED2 = ON; /* Turn ON LED2 */
        };
        
        if(varCount >= SELFTESTTIME)
        {
            varSelfTest = 1;
            varSwitch = 1;   /* Disable Transient Transition */
        }
    
    }    

}


/*******************************************************************************
Function: 	LoadControl
Description:	Controls transient response MOSFETs based on setting
*******************************************************************************/
void LoadControl(void)
{
    /* LED1 ON for Load Period */
    /* Load Pulldown and Pullup */
    /* Transient duration decided by LDTRANSTIME */
    /* Transient frequency decided by LDPERIOD */
        
        LED1 = ON;           /* Turn ON GREEN LED */
        
        #if(LOADSW == PULLDOWN)
            LOADSWGND = 1;
        #else
            LOADSWVCC = 0;
        #endif
        __delay_us(LDTRANSTIME);
        
        #if(LOADSW == PULLDOWN)
            LOADSWGND = 0;
        #else       
            LOADSWVCC = 1;
        #endif
        
        StartTimer1();
        while(varCount <= LDPERIOD) 
        {
            if(!SW1)
            {
                varSwitch = 1;      /* Stop Transient */
                LED1 = OFF;         /* Turn OFF GREEN LED */     
                __delay_ms(10);     /* Prevent Multiple Switch Presses */
            }
        }
                
 };

/*******************************************************************************
Function: 	SelfTest
Description:	Self Test Routine for Hardware Verification
*******************************************************************************/
void SelfTest(void)
{
    __delay_ms(SELFTESTTIME>>2);
    
    /* Check for output voltage */
    if((varVoltage <= varVoltageTripH) && (varVoltage >= varVoltageTripL))
    {
        varSelfTest = 0; /* Set variable to 0 */
        LED2 = OFF; /* Turn OFF LED2 */
        BlinkLED(3); /* Blink LED1 3 times */
    }
    else
    {        
        while(1)   /* Loop Continuously */
        {
            __delay_ms(SELFTESTTIME>>1)
            LED2=ON;
            __delay_ms(SELFTESTTIME>>1)
            LED2=OFF;
        }
    }
    
  
  
};
 
/*******************************************************************************
Function: 	SelfTest
Description:	Self Test Routine for Hardware Verification
*******************************************************************************/
void BlinkLED(uint8_t varTime)
{
  for(varTemp=0;varTemp<varTime;varTemp++)
    {
    __delay_ms(SELFTESTTIME>>1)
    LED1=ON;
    __delay_ms(SELFTESTTIME>>1)
    LED1=OFF;
    }
};

/*******************************************************************************
Function: 	StartTimer1
Description:	Start Timer1 for timing
*******************************************************************************/
void StartTimer1(void)
{
    varCount=0;
    T1CONbits.TON = 1;      /* Start Timer1 */
};

/*******************************************************************************
Function: 	StopTimer1
Description:	Stop Timer1
*******************************************************************************/
void StopTimer1(void)
{
    T1CONbits.TON = 0;      /* Stop Timer1 */
};
/*******************************************************************************
End of Function
*******************************************************************************/