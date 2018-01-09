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

#include <p33Exxxx.h>
#include <stdint.h>
#include <libpic30.h>
#include "../h/user.h"

#if(VPWMSELECT == PWM1)
    #include "filter1_dcdt.h"
#elif(VPWMSELECT == PWM2)
    #include "filter2_dcdt.h"
#elif(VPWMSELECT == PWM3)
    #include "filter3_dcdt.h"
#elif(VPWMSELECT == PWM4)
    #include "filter4_dcdt.h"
#endif

/******************************************************************************/
/* Clock Settings (MIPS)                                                      */    
/******************************************************************************/
#define SYS_FREQ        7370000UL
#define FOSC            ((float)SYS_FREQ*76)/4  /*140MHz*/
#define FCY             FOSC/2                  /*70MHz*/

/******************************************************************************/
/* Delay Routines                                                             */    
/******************************************************************************/
#define __delay_ms(d) \
  __delay32( (unsigned long) (((unsigned long long) d)*(FCY)/1000ULL)); 
#define __delay_us(d) \
  __delay32( (unsigned long) (((unsigned long long) d)*(FCY)/1000000ULL));

/******************************************************************************/
/* PWM Settings                                                               */    
/******************************************************************************/
#define PWMFREQUENCY 100000U                        /* 100kHz */

#define PWMCLKDIVBY 1U                              /* Value of CLKDIV divider */
#define F_ACLK  (unsigned long)(SYS_FREQ * 128U)    /* 943,718,400 ( = 8 x Auxiliary Clock Frequency) */
#define T_ACLK  ((float)(1/(float)(F_ACLK/PWMCLKDIVBY)))
#define PWMRESOLUTION_REG 1U        /* 1=1.06ns, 2=2.12ns, 3=4.24ns, 4=8.48ns, Used in PTCON2 register */
#define PWMPERIOD  ((unsigned int)(((float)((1/(float)(PWMFREQUENCY))/T_ACLK)-9))) /* PWM Period 9424 */

#define LEBDLY   200        /* Value in ns */
#define LEBDLY_REG  (int) (LEBDLY/(1.06*PWMCLKDIVBY))

/******************************************************************************/
/* TimerSettings                                                              */    
/******************************************************************************/
#define TMR_uS_SCALER   (FCY/1000000)   /* 1us/(1/Fcy) */
#define TMRPERIOD_uS    1000            /* in us */
#define TMR1SCALER      1               /* 0=1:1, 1=1:8, 2=1:64, 3=1:256 */
#define TMR1PERIOD     (unsigned int)(TMRPERIOD_uS*(float)(TMR_uS_SCALER)/8)

#define TMR2PERIOD (TMR1PERIOD*100)     /* 100 ms */

/******************************************************************************/
/* IO Settings                                                                */    
/******************************************************************************/
#define LOADSWGNDTRIS TRISEbits.TRISE0 /* Define PWM8L pin direction */
#define LOADSWVCCTRIS TRISEbits.TRISE1 /* Define PWM8H pin direction */
#define LED1TRIS TRISEbits.TRISE11  /* Define LED1 pin direction */
#define LED2TRIS TRISEbits.TRISE10  /* Define LED2 pin direction */
#define SW1TRIS TRISEbits.TRISE4    /* Define SW1 pin direction */

#define LOADSWGND LATEbits.LATE0    /* Define PWM8L pin */
#define LOADSWVCC LATEbits.LATE1    /* Define PWM8H pin */
#define LED1 LATEbits.LATE11        /* Define LED1 pin */
#define LED2 LATEbits.LATE10        /* Define LED2 pin */
#define SW1 PORTEbits.RE4           /* Define SW1 pin */

/******************************************************************************/
/* General Settings                                                           */    
/******************************************************************************/
#define ADCGAIN 1241       /* 1*2^12/3.3 */
#define VIN 3.3F
#define VOUTADC VOUT*ADCGAIN
#define VOUTMAX 3.3
#define VOUTMIN 0
#define VOUTMAXADC VOUTMAX*ADCGAIN
#define VOUTMINADC VOUTMIN*ADCGAIN
#define VOUTOFFSETADC 409       /* 10% max value */

#define OPENLOOPDC 0    /* Default dutycycle */
#define NOM_DC ((float)(VOUT/VIN))
#define MAXDUTYCYCLE 0.95*PWMPERIOD

#define SSCOUNT VOUTADC
#define DELAY_ADJ 0
#define SSTICKS (int)(((float)SSTIME*1000/(float)SSCOUNT)-DELAY_ADJ)

#define GATEDRV_DLY 0x0     /* To be set based on actual gate drive delay */

#define DBG 1

/******************************************************************************/
/* Global Variables                                                         */
/******************************************************************************/
extern volatile int16_t varCount,varState;
extern volatile int16_t varVoltageRef, varVoltageTripH, varVoltageTripL;
extern int16_t varVoltage;
extern int16_t varSelfTest,varTransientMode,varSwitch;
extern int16_t voltagePostScaler, voltagePostShift;
extern int16_t voltageMinClamp,voltageMaxClamp;
extern uint16_t voltagePWM,voltageTRIG,varFaultRef,varOpenLoopVoltage;
extern int16_t voltageErrorControlHistory[4];

/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/
void initClock(void); /* Handles clock switching/osc initialization */

/* Peripheral Initialization Subroutines */
void initIOPorts(void);         
void initTimer(void);
void initPWM(void);
void initADC(void);
void calibrateADC(void);
void initComp(void);
void initCMP(void);

void AltWREG1Setup(void);
void SMPS_Controller2P2ZUpdate_HW_Accel(void);

#define MACRO_CLR_VOLTAGEHISTORY() \
    voltageErrorControlHistory[0] = 0;\
    voltageErrorControlHistory[1] = 0;\
    voltageErrorControlHistory[2] = 0;\
    voltageErrorControlHistory[3] = 0;

#define MACRO_CALL_CONTROLLER()\
__asm("ctxtswp #1 \n"  \
       "call _SMPS_Controller2P2ZUpdate_HW_Accel \n" \
       "ctxtswp #0 \n" \
       ::: "cc","memory");

  
#if(VPWMSELECT==PWM1)
/* Filter 1 Compensator Coefficient Definition */
#define VOLTAGE_COMP_2P2Z_COEFF_A1   FILTER1_COMP_2P2Z_COEFF_A1      
#define VOLTAGE_COMP_2P2Z_COEFF_A2   FILTER1_COMP_2P2Z_COEFF_A2      
#define VOLTAGE_COMP_2P2Z_COEFF_B0   FILTER1_COMP_2P2Z_COEFF_B0      
#define VOLTAGE_COMP_2P2Z_COEFF_B1   FILTER1_COMP_2P2Z_COEFF_B1      
#define VOLTAGE_COMP_2P2Z_COEFF_B2   FILTER1_COMP_2P2Z_COEFF_B2      
#define VOLTAGE_COMP_2P2Z_POSTSCALER FILTER1_COMP_2P2Z_POSTSCALER       
#define VOLTAGE_COMP_2P2Z_POSTSHIFT  FILTER1_COMP_2P2Z_POSTSHIFT
#define VOLTAGE_COMP_2P2Z_PRESHIFT   FILTER1_COMP_2P2Z_PRESHIFT    
#define VOLTAGE_COMP_2P2Z_MIN_CLAMP  FILTER1_COMP_2P2Z_MIN_CLAMP
#define VOLTAGE_COMP_2P2Z_MAX_CLAMP  FILTER1_COMP_2P2Z_MAX_CLAMP
#define PDC PDC1
#define TRIG TRIG1

#elif(VPWMSELECT==PWM2)
/* Filter 2 Compensator Coefficient Definition */
#define VOLTAGE_COMP_2P2Z_COEFF_A1   FILTER2_COMP_2P2Z_COEFF_A1      
#define VOLTAGE_COMP_2P2Z_COEFF_A2   FILTER2_COMP_2P2Z_COEFF_A2      
#define VOLTAGE_COMP_2P2Z_COEFF_B0   FILTER2_COMP_2P2Z_COEFF_B0      
#define VOLTAGE_COMP_2P2Z_COEFF_B1   FILTER2_COMP_2P2Z_COEFF_B1      
#define VOLTAGE_COMP_2P2Z_COEFF_B2   FILTER2_COMP_2P2Z_COEFF_B2      
#define VOLTAGE_COMP_2P2Z_POSTSCALER FILTER2_COMP_2P2Z_POSTSCALER       
#define VOLTAGE_COMP_2P2Z_POSTSHIFT  FILTER2_COMP_2P2Z_POSTSHIFT
#define VOLTAGE_COMP_2P2Z_PRESHIFT   FILTER2_COMP_2P2Z_PRESHIFT    
#define VOLTAGE_COMP_2P2Z_MIN_CLAMP  FILTER2_COMP_2P2Z_MIN_CLAMP
#define VOLTAGE_COMP_2P2Z_MAX_CLAMP  FILTER2_COMP_2P2Z_MAX_CLAMP
#define PDC PDC2
#define TRIG TRIG2

#elif(VPWMSELECT==PWM3)
/* Filter 3 Compensator Coefficient Definition */
#define VOLTAGE_COMP_2P2Z_COEFF_A1   FILTER3_COMP_2P2Z_COEFF_A1      
#define VOLTAGE_COMP_2P2Z_COEFF_A2   FILTER3_COMP_2P2Z_COEFF_A2      
#define VOLTAGE_COMP_2P2Z_COEFF_B0   FILTER3_COMP_2P2Z_COEFF_B0      
#define VOLTAGE_COMP_2P2Z_COEFF_B1   FILTER3_COMP_2P2Z_COEFF_B1      
#define VOLTAGE_COMP_2P2Z_COEFF_B2   FILTER3_COMP_2P2Z_COEFF_B2      
#define VOLTAGE_COMP_2P2Z_POSTSCALER FILTER3_COMP_2P2Z_POSTSCALER       
#define VOLTAGE_COMP_2P2Z_POSTSHIFT  FILTER3_COMP_2P2Z_POSTSHIFT
#define VOLTAGE_COMP_2P2Z_PRESHIFT   FILTER3_COMP_2P2Z_PRESHIFT    
#define VOLTAGE_COMP_2P2Z_MIN_CLAMP  FILTER3_COMP_2P2Z_MIN_CLAMP
#define VOLTAGE_COMP_2P2Z_MAX_CLAMP  FILTER3_COMP_2P2Z_MAX_CLAMP
#define PDC PDC3
#define TRIG TRIG3

#elif(VPWMSELECT==PWM4)
/* Filter 4 Compensator Coefficient Definition */
#define VOLTAGE_COMP_2P2Z_COEFF_A1   FILTER4_COMP_2P2Z_COEFF_A1      
#define VOLTAGE_COMP_2P2Z_COEFF_A2   FILTER4_COMP_2P2Z_COEFF_A2      
#define VOLTAGE_COMP_2P2Z_COEFF_B0   FILTER4_COMP_2P2Z_COEFF_B0      
#define VOLTAGE_COMP_2P2Z_COEFF_B1   FILTER4_COMP_2P2Z_COEFF_B1      
#define VOLTAGE_COMP_2P2Z_COEFF_B2   FILTER4_COMP_2P2Z_COEFF_B2      
#define VOLTAGE_COMP_2P2Z_POSTSCALER FILTER4_COMP_2P2Z_POSTSCALER       
#define VOLTAGE_COMP_2P2Z_POSTSHIFT  FILTER4_COMP_2P2Z_POSTSHIFT
#define VOLTAGE_COMP_2P2Z_PRESHIFT   FILTER4_COMP_2P2Z_PRESHIFT    
#define VOLTAGE_COMP_2P2Z_MIN_CLAMP  FILTER4_COMP_2P2Z_MIN_CLAMP
#define VOLTAGE_COMP_2P2Z_MAX_CLAMP  FILTER4_COMP_2P2Z_MAX_CLAMP
#define PDC PDC4
#define TRIG TRIG4

#endif