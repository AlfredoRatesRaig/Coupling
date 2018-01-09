;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; © 2015 Microchip Technology Inc.
;
; MICROCHIP SOFTWARE NOTICE AND DISCLAIMER:  You may use this software, and any
; derivatives created by any person or entity by or on your behalf, exclusively
; with Microchip?s products.  Microchip and its licensors retain all ownership
; and intellectual property rights in the accompanying software and in all
; derivatives here to.
;
; This software and any accompanying information is for suggestion only.  It
; does not modify Microchip?s standard warranty for its products.  You agree
; that you are solely responsible for testing the software and determining its
; suitability.  Microchip has no obligation to modify, test, certify, or
; support the software.
;
; THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER
; EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED
; WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR
; PURPOSE APPLY TO THIS SOFTWARE, ITS INTERACTION WITH MICROCHIP?S PRODUCTS,
; COMBINATION WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.
;
; IN NO EVENT, WILL MICROCHIP BE LIABLE, WHETHER IN CONTRACT, WARRANTY, TORT
; (INCLUDING NEGLIGENCE OR BREACH OF STATUTORY DUTY), STRICT LIABILITY,
; INDEMNITY, CONTRIBUTION, OR OTHERWISE, FOR ANY INDIRECT, SPECIAL, PUNITIVE,
; EXEMPLARY, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, FOR COST OR EXPENSE OF
; ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWSOEVER CAUSED, EVEN IF
; MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.
; TO THE FULLEST EXTENT ALLOWABLE BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL
; CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF
; FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
;
; MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
; TERMS.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


; Local inclusions.
    .nolist
    .list

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;        .section .libdsp, code     ; use this section type when used as hidden library file
        .section .text              ; use this section type for debugging

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; _SMPS_Controller2P2ZUpdate_HW_Accel:

; Prototype:

;            void SMPS_Controller2P2ZUpdate_HW_Accel(void);
;
; Operation:
;                                          Wcp0   ( S + Wcz1 )
;  Calculate the transfer function Hc(s) = ---- X ------------
;                                           S     ( S + Wcp1 )
;
;
;
;                                  --                                                   ----------
;                                 |Bo|                                                 |postScalar|
;                                  --                                                   ----------
;Reference                          |                                                       |                                                    ------------------
;Input                              |                                    -------------      |           ------------                            |   SYSTEM PLANT   |
;        -   e[n]   ----------      -   e[n]*Bo   -      ---------      |    Sat      |     -          |  Sat       |      --------    u[n]     |                  |
;  o----|-|------->| preShift |--->|x|---------->|+|--->|postShift|---->|(Rnd(ACCAH)) |--->|x|-------->|(Rnd(ACCAH))|---->|Clamping|---------- >0   PDCx Update    |   
;        -    |     ----------      -             -      ---------       -------------      -           ------------   |   --------     |       |                  |
;        |    |                                   ^                                                                    |                |       |  Voltage/Current |
;        |    |                                   |                                                                    |                |       |    Feedback      |
;        |    |                                   |                                                                    |                |       |       |          |
;        |    |                                    <-----------------------------<              u[n] pre-clamping      |                |       |       |          |
;        |    |                                                                   |          <------------------------<                 |        -------0----------
;        |    |                                                                   |         |                                           |               |
;        |    |                                                                   |         |                                           |               |
;        |    |                                                                   |         |                       -------------------                 |
;        |    |                                                                   |         |                      |                                    |
;Measured|    |         ------     e[n-1]      ------    e[n-2]                   |         |                      | (Selectable by user)               |
;Input   |     ------->| z^-1 |-------------->| z^-1 |-----------                 |         |                      |                                    |
;        |              ------       |         ------            |                |         |        ----------    |     ---------                      |
;        |                           |                           |                |         |       |  Trigger |<------>| Trigger |                     |
;        |                           -                           -                |         |       | Off-Time |        | On-Time |                     |
;        |                   B1 --->|x|                 B2 ---->|x|               |         |        ----------          ---------                      |
;        |                           -                           -                |         |             |                  |                          |
;        |                           |                           |                |         |             |                  |                          |
;        |                           |                           |                |         |             |                  |                          |
;        |                     ----------------------------------------------     |         |              -------->-<-------                           |
;        |                    |                                              |    |         |                       |                                   |
;        |                    |            A C C U M U L A T O R             |--->          |                       |                                   |
;        |                    |                                              |              |                       |                                   |
;        |                     ----------------------------------------------               |                   ---------                               |
;        |                           |                           |                          |                  |  TRIGx  |                              |
;        |                           |                           |                          |                   ---------                               |
;        |                          |x|<---- A2                 |x|<---- A1                 |                                                           |
;        |                           -                           -                          |                                                           |
;        |                           |                           |                          |                                                           |
;        |                           |     ------                |    ------                |                                                           |
;        |                            ----| z^-1 |<------------------| z^-1 |<-------------<                                                            |
;        |                       u[n-2]    ------              u[n-1] ------                                                                            |
;        |                                                                                                                                              |
;        |                                                                                                                                              |
;         <--------------------------------------------------------------------------------------------------------------------------------------------<
;                                                                                                    Measured Output
;
;   2P2Z Compensator Function:
;
;   u[n] = B0*e[n] + B1*e[n-1] + B2*e[n-2] + A1*u[n-1] + A2*u[n-2]
;
;
;  where:
;
;   xABCoefficients[0]  = B0
;   xABCoefficients[1]  = B1
;   xABCoefficients[2]  = B2

;   xABCoefficients[3]  = A1
;   xABCoefficients[4]  = A2

;   xErrorControlHistory[0] = e[n-1]
;   xErrorControlHistory[1] = e[n-2]

;   xErrorControlHistory[2] = u[n-1]
;   xErrorControlHistory[3] = u[n-2]

;   where 'x' is for the converter name
;
;   Note that e[n] and u[n] are obtained during the critical path, prior to the MAC
;   instructions and thus not shown in the array assignments above.  After the critical
;   path, they become e[n-1] and u[n-1] for the subsequent iteration.
;
;   Function call inputs:
;   none
;
    
    .global _SMPS_Controller2P2ZUpdate_HW_Accel    ; Provide global scope to routine

_SMPS_Controller2P2ZUpdate_HW_Accel:
    push CORCON
    
    ; Immediately load previous ACCUMULATOR results to free up Alt-W registers w3, w4, & w5 for misc. instructions

    mov w3, ACCAL     ; Load bits 15:0
    mov w4, ACCAH     ; Load bits 31:16
    mov w5, ACCAU     ; Load bits 39:32

    mov  #0xA0, w4    
    mov  w4, _CORCON
    
    ; Calculate the most recent error with normalization

    mov [w0], w4      ; Move the contents of control reference to working register 
    sub w4, [w1], w5  ; w5 = Control Reference - ADCBUFx  ( w5 = w4 - [w1] )

    
    mov w5, [w10]     ; Store most recent error; becomes e[n-1] for next iteration

    mov [w9++], w4    ; w4 = B0
    mac w4*w5,  a     ; ACCA = ( B0 * e[n] ) + Previously ACCA stored results


    sftac a, w7     ; w7 = Normalization shift value to compensate coefficient scaling
                      ; ACC = ACC>>PostShift or ACC = ACC<<PostShift depending on sign of value

    sac.r a, #0, w4   ; w4 = Sat(Rnd(ACCAH)) - this is the control output value from previous
                      ; line of code after having been shifted.  Store bits 31:16 of the ACCA
                      ; into w4 temp register

    mpy   w4*w6, a    ; Multiply control output (after rounding) and postScalar factor
    sac.r a, w4       ; w4 = Sat(Rnd(ACCAH) again after normalization

   
    ; Save control output to control history array prior to clamping

    mov w4, [w10 + #0x4]    ; w4 = u[n] ( becomes u[n-1] for next calculation )

    ; Clamp to minimum if needed
    cpsgt w4, w11     ; Check if u[n] > minClamp.  If true, the next instruction is discarded and a Nop() is executed
                     ; If not true, execute next instruction
    mov.w w11, w4     ; Update u[n] with minClamp value

    ; Clamp to maximum if needed
    cpslt w4, w12    ; Check if u[n] < maxClamp. If true, the next instruction is discarded and a Nop() is executed
                     ; If not true, execute next instruction
    mov.w w12, w4    ; Update u[n] with maxClamp value

    ; Update the target register (Output):  [w2] = PFC Output variable
    mov w4, [w2]

       
    ; ------------------------------- End of Critical Path ------------------------------------------ ;
   
   ; Update control history:

    clr  a,        [w9]+=2, w4, [w10]+=2, w5     ; ACCA = EMPTY
                                                 ; w4   = B1, w5 = e[n-1]

    mac  w4*w5, a, [w9]+=2, w4, [w10]+=2, w5     ; ACCA = B1 * e[n-1]
                                                 ; w4   = B2, w5 = e[n-2]

    ; Calculate second section of 2P2Z controller -> (ACoefficients * controlHistory) and add results to accumulator 'A'

    mac  w4*w5, a, [w9]+=2, w4, [w10]+=2, w5     ; ACCA = B1 * e[n-1] + B2 * e[n-2] 
                                                 ; w4 = A1, w5 = u[n-1]

    mac  w4*w5, a, [w9]-=2, w4, [w10],    w5     ; ACCA = A1 * u[n-1] + B1 * e[n-1] + B2 * e[n-2] + B3 * e[n-3]
                                                 ; w4 = A2, w5 = u[n-2]

    mac  w4*w5, a, [w9]-=6, w4,  [w10]-=6, w5    ; ACCA = A1 * u[n-1] + A2 * u[n-2] + B0 * e[n] + B1 * e[n-1] + B2 * e[n-2]
                                                 ; Decrement w10 (error/control History) array back to base element
						 ; Decrement W9 array back to base
                         
    ; Update compensator errorControlHistory array elements

    mov [w10 + #4], w3      ; w3 = u[n-1]
    mov  w3, [w10 + #6]     ; u[n-2] = w3

    mov [w10], w3       ; w3 = e[n-1]
    mov  w3, [w10 + #2]     ; e[n-2] = w3

    ; Save 40-bit accumulator results for next iteration

    mov ACCAL, w3   ; Load bits: 15:0
    mov ACCAH, w4   ; Load bits: 31:16
    mov ACCAU, w5   ; Load bits: 39:32

    pop CORCON
    
    return
           
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  .end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; EOF
