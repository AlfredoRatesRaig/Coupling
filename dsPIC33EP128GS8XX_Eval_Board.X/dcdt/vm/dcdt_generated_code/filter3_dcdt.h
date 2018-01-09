/**
  Summary:
  This header file was created using Microchip Digital Compensator Design Tool (DCDT)
  and includes system summary and digital compensator coefficients. This file
  should be included in the MPLAB X project directory.

  File Name:     filter3_dcdt.h
  Project Name:  vm
  Date:          10/10/2016
  Time:          11:36.34

  Software License Agreement

  Copyright © 2016 Microchip Technology Inc.  All rights reserved.
  Microchip licenses to you the right to use, modify, copy and distribute
  Software only when embedded on a Microchip microcontroller or digital
  signal controller, which is integrated into your product or third party
  product (pursuant to the sublicense terms in the accompanying license
  agreement).

  You should refer to the license agreement accompanying this Software
  for additional information regarding your rights and obligations.

  SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY
  OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR
  PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR
  OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,
  BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT
  DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL,
  INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA,
  COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY
  CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF),
  OR OTHER SIMILAR COSTS.
 **/
#ifndef _FILTER3_DCDT_DEFINES_H
#define _FILTER3_DCDT_DEFINES_H

/**
  Compensator Type:  2P2Z
      Entry                Value  
    ---------            ---------
  Pole 0                 2.5000e+03 Hz
  Pole 2                 3.5000e+04 Hz
  Zero 1                 1.5915e+03 Hz
  Gain(Kdc)              1.000
  Warp                   false
  PWM Frequency          1.0000e+05
  PWM Sampling Ratio     1
  Sampling Frequency     1.0000e+05
  PWM Max Resolution     1.0600e-09
  Computational Delay    1.3500e-06
  Gate Drive Delay       1.5000e-09
  Control Output Min.    0
  Control Output Max.    32767
  Kuc Gain               7.6025e+00
  Use Kuc Gain           false


  PWM Calculations
      Name                Value  
    ---------           ---------
  Bits of Resolution    13.204
  Gain                  1.060e-04


  s-domain transfer function

               Wp0   Wp2(Wz1 + s)
  H(s) = Kdc X --- X ------------
                s    Wz1(Wp2 + s)

                  1.57e+04   2.20e+05(1.00e+04 + s)
  H(s) = 1.000 X -------- X ----------------------
                     s       1.00e+04(2.20e+05 + s)



  Digital Compensator Coefficients

  Name    Value     Normalized    Q15      Hex
  ----    -----     ----------    ---      ---
  a1      0.953     0.953         31214    0x79EE
  a2      0.047     0.047         1553     0x0611
  b0      0.864     0.864         28304    0x6E90
  b1      0.082     0.082         2695     0x0A87
  b2      -0.782    -0.782        -25609   0x9BF7


  z-domain transfer function

         u(z)  B0 + B1z^(-1) + B2z^(-2)
  H(z) = --- = ------------------------
         e(z)  A0 - A1z^(-1) - A2z^(-2)

          (0.864) + (0.082)z^(-1) + (-0.782)z^(-2)
  H(z) = ---------------------------------------------
          1 - (0.953)z^(-1) - (0.047)z^(-2)

**/


/* Compensator Coefficient Defines */
#define FILTER3_COMP_2P2Z_COEFF_A1      0x79EE
#define FILTER3_COMP_2P2Z_COEFF_A2      0x0611
#define FILTER3_COMP_2P2Z_COEFF_B0      0x6E90
#define FILTER3_COMP_2P2Z_COEFF_B1      0x0A87
#define FILTER3_COMP_2P2Z_COEFF_B2      0x9BF7
#define FILTER3_COMP_2P2Z_POSTSCALER    0x7FFF
#define FILTER3_COMP_2P2Z_POSTSHIFT     0x0000
#define FILTER3_COMP_2P2Z_PRESHIFT      0x0000


/* Compensator Clamp Limits */
#define FILTER3_COMP_2P2Z_MIN_CLAMP    0x0000
#define FILTER3_COMP_2P2Z_MAX_CLAMP    0x7FFF


#endif
