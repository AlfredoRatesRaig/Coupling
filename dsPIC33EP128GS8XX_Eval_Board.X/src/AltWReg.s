
.include "p33Exxxx.inc"
#include "init.h"
#include "user.h"
    
.data

.text

.section .text._AltWREG1Setup, keep, code
.global _AltWREG1Setup

_AltWREG1Setup:

;Swap to Alternate W-Reg #1
CTXTSWP #0x1

mov #_varVoltageRef, w0	    ;w0 = Address of varVoltageRef
mov #_ADCBUF0, w1	    ;w1 = Address of ADCBUF0
mov _voltagePWM, w2	    ;w2 = voltagePWM

; W3, W4, W5 used for ACCAx, MAC/MPY instructions
   
mov _voltagePostScaler, w6  ;w6 = voltagePostScaler
mov _voltagePostShift, w7   ;w7 = voltagePostShift

mov #_voltageABCoefficients, w9		
mov #_voltageErrorControlHistory, w10

mov _voltageMinClamp, w11   ;w11 = voltageMinClamp
mov _voltageMaxClamp, w12   ;w11 = voltageMaxClamp

;W13, W14 open for use in ISR
    
; Swap back to main register set
CTXTSWP #0x0

 return
 
.end
