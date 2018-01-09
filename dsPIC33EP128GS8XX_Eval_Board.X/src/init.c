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

#include "../h/init.h"

/*******************************************************************************
Global variables
*******************************************************************************/
int16_t voltageABCoefficients[5]__attribute__ ((section (".xbss")));
int16_t voltageErrorControlHistory[4] __attribute__ ((space (ymemory), far));

int16_t voltagePostScaler,voltagePostShift;
int16_t voltageMinClamp,voltageMaxClamp;
uint16_t voltagePWM,voltageTRIG;
int16_t varGateDelay = GATEDRV_DLY; 
int16_t varVoltage;

volatile int16_t varVoltageRef=0, varVoltageTripH=0, varVoltageTripL=0;
volatile int16_t varState;
uint16_t varFaultRef = VOUTMAXADC;

/*******************************************************************************
Function: 	initClock
Description:	Initialize system clock and PWM/ADC clock
*******************************************************************************/
void initClock(void)
{
    /* Configure Oscillator to operate the device at 70Mhz */
    /* Fosc = Fin*M/(N1*N2), Fcy = Fosc/2 */
    /* Fosc = 7.37*(76)/(2*2)= 140Mhz for Fosc, Fcy = 70Mhz */

    /* Configure PLL prescaler, PLL postscaler, PLL divisor */
    PLLFBD             = 74;   /* M = PLLFBD + 2 */
    CLKDIVbits.PLLPOST = 0;     /* N1 = 2 */
    CLKDIVbits.PLLPRE = 0;      /* N2 = 2 */

    /* Change oscillator to FRC + PLL */
    __builtin_write_OSCCONH(0x01);      /* New Oscillator FRC w/ PLL */
    __builtin_write_OSCCONL(0x01);  	/* Enable Clock Switch */

    while(OSCCONbits.COSC != 0b001);	/* Wait for new Oscillator - FRC w/ PLL */
    while(OSCCONbits.LOCK != 1);        /* Wait for Pll to Lock */

    /* Now setup the ADC and PWM clock for 120MHz */
    /*   ((FRC * 16) / APSTSCLR ) = (7.37 * 16) / 1 = ~ 120MHz */

    ACLKCONbits.FRCSEL = 1;     /* FRC provides input to Auxiliary PLL (x16) */
    ACLKCONbits.SELACLK = 1;	/* Auxiliary Oscillator provides clock */
                                /* source for PWM & ADC */
    ACLKCONbits.APSTSCLR = 7;   /* Divide Auxiliary clock by 1 */
    ACLKCONbits.ENAPLL = 1;     /* Enable Auxiliary PLL */
    while(ACLKCONbits.APLLCK != 1);	/* Wait for Aux. PLL to Lock */
//    __delay_us(50);
}

/*******************************************************************************
Function: 	initPWM
Description:	Initialize PWM modules
*******************************************************************************/
void initPWM(void)
{
    PTCONbits.PTEN = 0;     /* Disable the PWM */ 
    
    PTCON2bits.PCLKDIV = (PWMRESOLUTION_REG-1); /* PWM Resolution */
    PTPER = PWMPERIOD;                  /* Set PWM frequency */
                      
    #if(VPWMSELECT==PWM1)
        IOCON1bits.PMOD = 1;            /* Redundant Mode */

        IOCON1bits.POLH = 0;            /* Drive signals are active-high */
        IOCON1bits.POLL = 0;            /* Drive signals are active-high */ 

        IOCON1bits.OVRENH = 0;          /* Override disabled */
        IOCON1bits.OVRENL = 0;	  
        IOCON1bits.OVRDAT = 0b00;       /* Override data PWMH and PWML */
        IOCON1bits.FLTDAT = 0b00;       /* If fault occurs: */
                                        /* PWMH = 0 & PWML = 0 */
        PWMCON1bits.DTC   = 0;          /* Positive Deadtime enabled */
        DTR1    = 80;
        ALTDTR1 = 110;

        PWMCON1bits.IUE = 0;            /* Disable Immediate duty cycle updates */
        PWMCON1bits.ITB = 0;            /* Select Primary Timebase mode */

        FCLCON1bits.CLSRC = 0b01101;   /* Fault Control Signal assigned to CMP1B */
        FCLCON1bits.CLPOL = 0;         /* Fault Signal is active-high */
        FCLCON1bits.CLMOD = 0b01;      /* Truncate Cycle */

        LEBCON1bits.PHR      = 1;       /* Enable LEB bit for HS MOSFET rising edge */
        LEBCON1bits.CLLEBEN = 1;        /* Fault Input LEB Enabled */
        LEBDLY1bits.LEB      = 20;      /* 8.32n Steps x 20 = 160ns */

        TRGCON1bits.TRGDIV  = 0;        /* Trigger interrupt generated every PWM cycle */

        TRGCON1bits.TRGSTRT = 0;        /* Trigger generated after waiting 0 PWM cycles */
        
        PDC1 = OPENLOOPDC;              /* Set default Open-loop Duty-cycle */
        
        TRIG1 = 30;                     /* Set Initial Trigger location */
        
              
    #elif(VPWMSELECT==PWM2)
       
        IOCON2bits.PMOD = 1;            /* Redundant Mode */

        IOCON2bits.POLH = 0;            /* Drive signals are active-high */
        IOCON2bits.POLL = 0;            /* Drive signals are active-high */

        IOCON2bits.OVRENH = 0;          /* Override disabled */
        IOCON2bits.OVRENL = 0;	  
        IOCON2bits.OVRDAT = 0b00;       /* Override data PWMH and PWML */
        IOCON2bits.FLTDAT = 0b00;       /* If fault occurs: */
                                        /* PWMH = 0 & PWML = 0 */
        PWMCON2bits.DTC   = 0;          /* Positive Deadtime enabled */
        DTR2    = 80;
        ALTDTR2 = 110;

        PWMCON2bits.IUE = 0;            /* Disable Immediate duty cycle updates */
        PWMCON2bits.ITB = 0;            /* Select Primary Timebase mode */

        FCLCON2bits.CLSRC = 0b01101;   /* Fault Control Signal assigned to CMP1B */
        FCLCON2bits.CLPOL = 0;         /* Fault Signal is active-high */
        FCLCON2bits.CLMOD = 0b01;      /* Truncate Cycle */

        LEBCON2bits.PHR      = 1;       /* Enable LEB bit for HS MOSFET rising edge */
        LEBCON2bits.CLLEBEN = 1;        /* Fault Input LEB Enabled */
        LEBDLY2bits.LEB      = 20;      /* 8.32n Steps x 20 = 160ns */

        TRGCON2bits.TRGDIV  = 0;        /* Trigger interrupt generated every PWM cycle */

        TRGCON2bits.TRGSTRT = 0;        /* Trigger generated after waiting 0 PWM cycles */

        PDC2 = OPENLOOPDC;              /* Set default Open-loop Duty-cycle */

        TRIG2 = 30;                     /* Set Initial Trigger location */
        
                
    #elif(VPWMSELECT==PWM3)
       
        IOCON3bits.PMOD = 1;            /* Redundant Mode */

        IOCON3bits.POLH = 0;            /* Drive signals are active-high */
        IOCON3bits.POLL = 0;            /* Drive signals are active-high */

        IOCON3bits.OVRENH = 0;          /* Override disabled */
        IOCON3bits.OVRENL = 0;	  
        IOCON3bits.OVRDAT = 0b00;       /* Override data PWMH and PWML */
        IOCON3bits.FLTDAT = 0b00;       /* If fault occurs: */
                                        /* PWMH = 0 & PWML = 0 */
        PWMCON3bits.DTC   = 0;          /* Positive Deadtime enabled */
        DTR3    = 80;
        ALTDTR3 = 110;

        PWMCON3bits.IUE = 0;            /* Disable Immediate duty cycle updates */
        PWMCON3bits.ITB = 0;            /* Select Primary Timebase mode */

        FCLCON3bits.CLSRC = 0b01101;   /* Fault Control Signal assigned to CMP1B */
        FCLCON3bits.CLPOL = 0;         /* Fault Signal is active-high */
        FCLCON3bits.CLMOD = 0b01;      /* Truncate Cycle */

        LEBCON3bits.PHR      = 1;       /* Enable LEB bit for HS MOSFET rising edge */
        LEBCON3bits.CLLEBEN = 1;        /* Fault Input LEB Enabled */
        LEBDLY3bits.LEB      = 20;      /* 8.32n Steps x 20 = 160ns */

        TRGCON3bits.TRGDIV  = 0;        /* Trigger interrupt generated every PWM cycle */

        TRGCON3bits.TRGSTRT = 0;        /* Trigger generated after waiting 0 PWM cycles */

        PDC3 = OPENLOOPDC;              /* Set default Open-loop Duty-cycle */

        TRIG3 = 30;                     /* Set Initial Trigger location */
        
               
              
    #elif(VPWMSELECT==PWM4)
               
        IOCON4bits.PMOD = 1;            /* Redundant Mode */

        IOCON4bits.POLH = 0;            /* Drive signals are active-high */
        IOCON4bits.POLL = 0;            /* Drive signals are active-high */

        IOCON4bits.OVRENH = 0;          /* Override disabled */
        IOCON4bits.OVRENL = 0;	  
        IOCON4bits.OVRDAT = 0b00;       /* Override data PWMH and PWML */
        IOCON4bits.FLTDAT = 0b00;       /* If fault occurs: */
                                        /* PWMH = 0 & PWML = 0 */
        PWMCON4bits.DTC   = 0;          /* Positive Deadtime enabled */
        DTR4    = 80;
        ALTDTR4 = 110;

        PWMCON4bits.IUE = 0;            /* Disable Immediate duty cycle updates */
        PWMCON4bits.ITB = 0;            /* Select Primary Timebase mode */

        FCLCON4bits.CLSRC = 0b01101;   /* Fault Control Signal assigned to CMP1B */
        FCLCON4bits.CLPOL = 0;         /* Fault Signal is active-high */
        FCLCON4bits.CLMOD = 0b01;      /* Truncate Cycle */

        LEBCON4bits.PHR      = 1;       /* Enable LEB bit for HS MOSFET rising edge */
        LEBCON4bits.CLLEBEN = 1;        /* Fault Input LEB Enabled */
        LEBDLY4bits.LEB      = 20;      /* 8.32n Steps x 20 = 160ns */

        TRGCON4bits.TRGDIV  = 0;        /* Trigger interrupt generated every PWM cycle */
        TRGCON4bits.TRGSTRT = 0;        /* Trigger generated after waiting 0 PWM cycles */

        PDC4 = OPENLOOPDC;              /* Set default Open-loop Duty-cycle */

        TRIG4 = 200;                    /* Set Initial Trigger location */
        
    #endif                
      
    #if(IPWMSELECT==PWM5)
               
        IOCON5bits.PMOD = 1;            /* Redundant Mode */

        IOCON5bits.POLH = 0;            /* Drive signals are active-high */
        IOCON5bits.POLL = 0;            /* Drive signals are active-high */

        IOCON5bits.OVRENH = 0;          /* Override disabled */
        IOCON5bits.OVRENL = 0;	  
        IOCON5bits.OVRDAT = 0b00;       /* Override data PWMH and PWML */
        IOCON5bits.FLTDAT = 0b00;       /* If fault occurs: */
                                        /* PWMH = 0 & PWML = 0 */
        PWMCON5bits.DTC   = 0;          /* Positive Deadtime enabled */
        DTR5    = 80;
        ALTDTR5 = 110;

        PWMCON5bits.IUE = 0;            /* Disable Immediate duty cycle updates */
        PWMCON5bits.ITB = 0;            /* Select Primary Timebase mode */
        
        FCLCON5bits.CLSRC = 0b01101;   /* Fault Control Signal assigned to CMP1B */
        FCLCON5bits.CLPOL = 0;         /* Fault Signal is active-high */
        FCLCON5bits.CLMOD = 0b01;      /* Truncate Cycle */
     
        PDC5 = PTPER>>1;              /* Set default Open-loop Duty-cycle */

        TRIG5 = 30;                     /* Set Initial Trigger location */
        
    #elif(IPWMSELECT==PWM6)
               
        IOCON6bits.PMOD = 1;            /* Redundant Mode */

        IOCON6bits.POLH = 0;            /* Drive signals are active-high */
        IOCON6bits.POLL = 0;            /* Drive signals are active-high */

        IOCON6bits.OVRENH = 0;          /* Override disabled */
        IOCON6bits.OVRENL = 0;	  
        IOCON6bits.OVRDAT = 0b00;       /* Override data PWMH and PWML */
        IOCON6bits.FLTDAT = 0b00;       /* If fault occurs: */
                                        /* PWMH = 0 & PWML = 0 */
        PWMCON6bits.DTC   = 0;          /* Positive Deadtime enabled */
        DTR6    = 80;
        ALTDTR6 = 110;

        PWMCON6bits.IUE = 0;            /* Disable Immediate duty cycle updates */
        PWMCON6bits.ITB = 0;            /* Select Primary Timebase mode */
        
        FCLCON6bits.CLSRC = 0b01101;   /* Fault Control Signal assigned to CMP1B */
        FCLCON6bits.CLPOL = 0;         /* Fault Signal is active-high */
        FCLCON6bits.CLMOD = 0b01;      /* Truncate Cycle */

        PDC6 = PTPER>>1;              /* Set default Open-loop Duty-cycle */

        TRIG6 = 30;                     /* Set Initial Trigger location */
        
    #endif    
           
    #if(RPWMSELECT==PWM7)
               
        IOCON7bits.PMOD = 1;            /* Redundant Mode */

        IOCON7bits.POLH = 0;            /* Drive signals are active-high */
        IOCON7bits.POLL = 0;            /* Drive signals are active-high */

        IOCON7bits.OVRENH = 0;          /* Override disabled */
        IOCON7bits.OVRENL = 0;	  
        IOCON7bits.OVRDAT = 0b00;       /* Override data PWMH and PWML */
        IOCON7bits.FLTDAT = 0b00;       /* If fault occurs: */
                                        /* PWMH = 0 & PWML = 0 */
        PWMCON7bits.DTC   = 0;          /* Positive Deadtime enabled */
        DTR7   = 80;
        ALTDTR7 = 110;

        PWMCON7bits.IUE = 0;            /* Disable Immediate duty cycle updates */
        PWMCON7bits.ITB = 0;            /* Select Primary Timebase mode */

        PDC7 = PTPER>>1;           /* Set default Open-loop Duty-cycle */    
        TRIG7 = 30;                     /* Set Initial Trigger location */
               
    #endif
        
};

/*******************************************************************************
Function: 	initADC
Description:	Initialize ADC module and ADC interrupts
*******************************************************************************/
void initADC(void)
{
    /* Setup ADC Clock Input Max speed of 70 MHz --> Fosc = 140 MHz */
    ADCON3Hbits.CLKSEL  = 1;    /* 0-Fsys, 1-Fosc, 2-FRC, 3-APLL */
    ADCON3Hbits.CLKDIV  = 0;    /* Global Clock divider (1:1) */
    ADCORE0Hbits.ADCS   = 0;    /* Core 0 clock divider (1:2) */
    ADCORE1Hbits.ADCS   = 0;    /* Core 1 clock divider (1:2) */
    ADCORE2Hbits.ADCS   = 0;    /* Core 2 clock divider (1:2) */
    ADCORE3Hbits.ADCS   = 0;    /* Core 3 clock divider (1:2) */
    ADCON2Lbits.SHRADCS = 0;    /* 1/2 clock divider */

    ADCON1Hbits.FORM    = 0;    /* Integer format */
    ADCON3Lbits.REFSEL  = 0;    /* AVdd as voltage reference */

     /* ADC Cores in 12-bit resolution mode */
    ADCON1Hbits.SHRRES  = 3;    /* Shared ADC Core in 12-bit res mode */
    ADCORE0Hbits.RES    = 3;    /* Core 0 ADC Core in 12-bit res mode */
    ADCORE1Hbits.RES    = 3;    /* Core 1 ADC Core in 12-bit res mode */
    ADCORE2Hbits.RES    = 3;    /* Core 2 ADC Core in 12-bit res mode */
    ADCORE3Hbits.RES    = 3;    /* Core 3 ADC Core in 12-bit res mode */
    ADCON2Hbits.SHRSAMC = 2;    /* Shared ADC Core sample time 4Tad */

    /* Configure ANx for unsigned format and single ended (0,0) */
    ADMOD0L = 0x0000;

    /* Calibrate ADC Modules */
    calibrateADC();
           
    /* ADC0 Settings */
    #if(VPWMSELECT==PWM1)    
        ADTRIG0Lbits.TRGSRC0 = 2;    /* ADC0 triggered by PWM1 */
    #elif(VPWMSELECT==PWM2)
        ADTRIG0Lbits.TRGSRC0 = 4;    /* ADC0 triggered by PWM2 */
    #elif(VPWMSELECT==PWM3)
        ADTRIG0Lbits.TRGSRC0 = 6;    /* ADC0 triggered by PWM3 */
    #elif(VPWMSELECT==PWM4)
        ADTRIG0Lbits.TRGSRC0 = 8;    /* ADC0 triggered by PWM4 */
    #endif
    IFS6bits.ADCAN0IF  = 0;      /* Clear ADC interrupt flag */
    IPC27bits.ADCAN0IP = 7;      /* Set ADC0 interrupt priority to 7 */
    ADIELbits.IE0  = 1;          /* Enable ADC0 Interrupt */
//   _ADCAN0IE = 1;  

    ADCON3Lbits.CNVCHSEL = 2;    /* Channel 2 manual trigger R12, R20 */
}

/*******************************************************************************
Function: 	calibrateADC
Description:	Calibrate ADC modules
*******************************************************************************/
void calibrateADC(void)
{
    /* Power Up delay: 2048 Core Clock Source Periods (TCORESRC) for all ADC cores */
    /* (~14.6 us) */
    _WARMTIME = 11;

    /* Turn on ADC module */
    ADCON1Lbits.ADON  = 1;
    
    /* Turn on analog power for dedicated core 0 */
    ADCON5Lbits.C0PWR = 1;
    while(ADCON5Lbits.C0RDY == 0);
    ADCON3Hbits.C0EN  = 1;   /* Enable ADC core 0 */

    /* Turn on analog power for dedicated core 1 */
    ADCON5Lbits.C1PWR = 1;
    while(ADCON5Lbits.C1RDY == 0);
    ADCON3Hbits.C1EN  = 1;   /* Enable ADC core 1 */

    /* Turn on analog power for dedicated core 2 */
    ADCON5Lbits.C2PWR = 1;
    while(ADCON5Lbits.C2RDY == 0);
    ADCON3Hbits.C2EN  = 1;   /* Enable ADC core 2 */

    /* Turn on analog power for dedicated core 3 */
    ADCON5Lbits.C3PWR = 1;
    while(ADCON5Lbits.C3RDY == 0);
    ADCON3Hbits.C3EN  = 1;   /* Enable ADC core 3 */

    /* Turn on analog power for shared core */
    ADCON5Lbits.SHRPWR = 1;
    while(ADCON5Lbits.SHRRDY == 0);
    ADCON3Hbits.SHREN  = 1;  /* Enable shared ADC core */

    /* Enable calibration for the dedicated core 0 */
    ADCAL0Lbits.CAL0EN   = 1;
    ADCAL0Lbits.CAL0DIFF = 0;         /* Single-ended input calibration */
    ADCAL0Lbits.CAL0RUN  = 1;         /* Start Cal */
    while(ADCAL0Lbits.CAL0RDY == 0);
    ADCAL0Lbits.CAL0EN   = 0;         /* Cal complete */

    /* Enable calibration for the dedicated core 1 */
    ADCAL0Lbits.CAL1EN   = 1;
    ADCAL0Lbits.CAL1DIFF = 0;         /* Single-ended input calibration */
    ADCAL0Lbits.CAL1RUN  = 1;         /* Start Cal */
    while(ADCAL0Lbits.CAL1RDY == 0);
    ADCAL0Lbits.CAL1EN   = 0;         /* Cal complete */

    /* Enable calibration for the dedicated core 2 */
    ADCAL0Hbits.CAL2EN   = 1;
    ADCAL0Hbits.CAL2DIFF = 0;         /* Single-ended input calibration */
    ADCAL0Hbits.CAL2RUN  = 1;         /* Start Cal */
    while(ADCAL0Hbits.CAL2RDY == 0);
    ADCAL0Hbits.CAL2EN   = 0;         /* Cal complete */

    /* Enable calibration for the dedicated core 3 */
    ADCAL0Hbits.CAL3EN   = 1;
    ADCAL0Hbits.CAL3DIFF = 0;         /* Single-ended input calibration */
    ADCAL0Hbits.CAL3RUN  = 1;         /* Start Cal */
    while(ADCAL0Hbits.CAL3RDY == 0);
    ADCAL0Hbits.CAL3EN   = 0;         /* Cal complete */

    /* Enable calibration for the shared core */
    ADCAL1Hbits.CSHREN   = 1;
    ADCAL1Hbits.CSHRDIFF = 0;        /* Single-ended input calibration */
    ADCAL1Hbits.CSHRRUN  = 1;        /* Start calibration cycle */
    while(ADCAL1Hbits.CSHRRDY == 0); /* while calibration is still in progress */

    ADCAL1Hbits.CSHREN   = 0;        /* Calibration is complete */       
};

/*******************************************************************************
Function: 	initIOPorts
Description:	Initialize all IO pins
*******************************************************************************/
void initIOPorts(void)
{
    /* Unused PWM, used as IO */
    IOCON8bits.PENH = 0;
    IOCON8bits.PENL = 0;
    
   /* Initialize IOs */
    LOADSWGNDTRIS = 0;      /* PWM8L pin set as IO rather than PWM */
    LOADSWVCCTRIS = 0;      /* PWM8H pin set as IO rather than PWM */
    LED1TRIS = 0;           /* Pin set as output to drive LED1 */
    LED2TRIS = 0;           /* Pin set as output to drive LED2 */
    SW1TRIS = 1;            /* Pin set as input to read SW1 */
    
    LOADSWGND = 0;          /* Default = 0; 1 to turn on, N-CH */
    LOADSWVCC = 1;          /* Default = 1; 0 to turn on, P-CH */
 
     /* For Debug */
    TRISEbits.TRISE7=0;
    
}

/*******************************************************************************
Function: 	initCMP
Description:	Initialize comparator modules
*******************************************************************************/
void initCMP(void)
{    
    /* Comparator 1 */
    CMP1CONbits.INSEL = 1;      /* CMP1B as input */
    CMP1CONbits.HYSSEL = 3;     /* Add hysteresis */
    CMP1CONbits.RANGE = 1;      /* AVDD */
    CMP1CONbits.ALTINP = 0;     /* Select regular input */
    CMP1DAC = 2048;             /* 3.3V default level */
//    CMP1CONbits.DACOE = 1;      /* Output on DACOUT pin */
      
//    RPOR15bits.RP60R = 24;      /* Monitor CMP1 output on RP60*/
    
        
    /* Comparator 4 */
    /* Use jumper J20-1 and select PWM1 */
    CMP4CONbits.INSEL = 0;      /* CMP4A as input */
    CMP4CONbits.HYSSEL = 3;     /* Add hysteresis */
    CMP4CONbits.RANGE = 1;      /* AVDD */
    CMP4CONbits.ALTINP = 0;     /* Select regular input */
    CMP4DAC = 4095;             /* Set level */
    CMP4CONbits.DACOE = 1;    /* Output on DACOUT pin */
    CMP4CONbits.CMPON = 1;      /* Comparator ON */
     
//    RPOR15bits.RP60R = 50;    /* Monitor CMP4 output on RP60*/
}

/*******************************************************************************
Function: 	initPGA
Description:	Initialize comparator modules
*******************************************************************************/
void initPGA(void)
{    
    /* EXTREF2 connected to PGA1P4 */
    PGA1CONbits.SELNI = 0;  /* GND */
    PGA1CONbits.SELPI = 3;  /* CH4 */
    PGA1CONbits.GAIN = 2;   /* Min. Gain = 4*/
    PGA1CONbits.PGAOEN = 1; /* Connect output to DACOUT */
    PGA1CONbits.PGAEN = 1;  /* Enable PGA */
                
}

/*******************************************************************************
Function: 	initTMR
Description:	Initialize Timers
*******************************************************************************/
void initTimer(void)
{
    PR1 = TMR1PERIOD;
    T1CONbits.TCKPS = TMR1SCALER;
    T1CONbits.TCS = 0;       /* Clock source is internal Fcy */

    IFS0bits.T1IF = 0;
    IPC0bits.T1IP = 4;
    IEC0bits.T1IE = 1;
//    T1CONbits.TON = 1;    /* Start Timer1 */
    
    PR2 = TMR2PERIOD;
    T2CONbits.TCKPS = TMR1SCALER;
    T2CONbits.TCS = 0;       /* Clock source is internal Fcy */

    IFS0bits.T2IF = 0;
    IPC1bits.T2IP = 4;
    IEC0bits.T2IE = 1;
//    T2CONbits.TON = 1;    /* Start Timer2 */
 };

/*******************************************************************************
Function: 	initComp
Description:	Initialize compensators 
*******************************************************************************/
void initComp(void)
{
    MACRO_CLR_VOLTAGEHISTORY();
    
    voltageABCoefficients[0] = VOLTAGE_COMP_2P2Z_COEFF_B0;
    voltageABCoefficients[1] = VOLTAGE_COMP_2P2Z_COEFF_B1;
    voltageABCoefficients[2] = VOLTAGE_COMP_2P2Z_COEFF_B2;
    
    voltageABCoefficients[3] = VOLTAGE_COMP_2P2Z_COEFF_A1;
    voltageABCoefficients[4] = VOLTAGE_COMP_2P2Z_COEFF_A2;
    
    voltagePostScaler = VOLTAGE_COMP_2P2Z_POSTSCALER;
    voltagePostShift  = VOLTAGE_COMP_2P2Z_POSTSHIFT;
    voltageMinClamp   = VOLTAGE_COMP_2P2Z_MIN_CLAMP;  
    voltageMaxClamp   = VOLTAGE_COMP_2P2Z_MAX_CLAMP;
    
    voltagePWM = (uint16_t)&PDC;
    voltageTRIG = (uint16_t)&TRIG;
        
    AltWREG1Setup(); /* Setup working registers to be used with compensator */
        
}

/*******************************************************************************
Function: 	initPMDBits
Description:	UnClock all unused modules
*******************************************************************************/
void initPMDBits(void)
{        
// 	  PMD1 = 0xD0D8;  /* T3,T1, PWM, U1, and ADC left clocked */
//    PMD2 = 0x0F0F;  /* All modules unclocked */
//    PMD3 = 0x0002;  /* CMP clocked */
//    PMD4 = 0x0008;  /* All modules unclocked */
//    PMD6 = 0x1C00;  /* PWM1 clocked */
//    PMD7 = 0x0002;  /* CMP1-CMP4 clocked */
//    PMD8 = 0x0602;  /* All modules unclocked */
}

/*******************************************************************************
End of Function
*******************************************************************************/