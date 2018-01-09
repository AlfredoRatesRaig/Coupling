.include "p33Exxxx.inc"
#include "../h/init.h"
    
.data
  
.text

.section .text._ADCAN0Interrupt, keep, code
.global __ADCAN0Interrupt

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; ISR:          ADCAN0Interrupt
; Working Reg:  Uses Alt W-Reg Bank #1
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
__ADCAN0Interrupt:

    mov ADCBUF0, w14	    ; Read Buffer for Debug 
    mov w14, _varVoltage    ; Save the variable 
  
    ; Function calls 2P2Z controllers
    call _SMPS_Controller2P2ZUpdate_HW_Accel
    
    ; Update Trigger (50% Trigger on-time)
    mov [w2], w14
    
    asr w14, #1, w14           ; Divide PDCx by 2
;    mov #GATEDRV_DLY, w13
    mov _varGateDelay, w13			
    add w14, w13, w13          ; Add delay to account for gate drive delay
    mov _voltageTRIG,w14
    mov w13, [w14]

    bclr IFS6, #14
    retfie 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   End of ISR
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.end
    