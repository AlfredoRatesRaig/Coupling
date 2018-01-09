/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

/* Device header file */
#if defined(__XC16__)
    #include <xc.h>
#elif defined(__C30__)
    #if defined(__dsPIC33E__)
    	#include <p33Exxxx.h>
    #elif defined(__dsPIC33F__)
    	#include <p33Fxxxx.h>
    #endif
#endif

#include <stdint.h>        /* Includes uint16_t definition   */
#include "../h/init.h" 

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/
void __attribute__((__interrupt__,no_auto_psv)) _T1Interrupt()
{
    varCount++;
    IFS0bits.T1IF = 0;  /* Clear Interrupt Flag */
};

void __attribute__((__interrupt__,no_auto_psv)) _T2Interrupt()
{
    #if(VLTFLTENABLED)  /* If fault is enabled */
        /* Check for output voltage */
        if((varVoltage >= varVoltageTripH) || (varVoltage <= varVoltageTripL))
        {
            varState = FAULT;
        }
    #endif

    IFS0bits.T2IF = 0;  /* Clear Interrupt Flag */
};

void __attribute__((__interrupt__,no_auto_psv)) _ADCAN0Interrupt()
{
    varVoltage = ADCBUF0;
    MACRO_CALL_CONTROLLER();
    TRIG = (PDC >> 1) + GATEDRV_DLY;     
    
    IFS6bits.ADCAN0IF = 0;  /* Clear Interrupt Flag */
}
