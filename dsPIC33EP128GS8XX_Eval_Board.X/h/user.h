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
/* User Function Prototypes                                                   */
/******************************************************************************/
#define ON 1
#define OFF 0
#define ENABLED 1
#define DISABLED 0

/* PWM Channel Definition */
#define PWM1 1
#define PWM2 2
#define PWM3 3
#define PWM4 4
#define PWM5 5
#define PWM6 6
#define PWM7 7

/* Voltage Loop PWM Selection */
/* Select between PWMs 1-4 */
/* Jumper Position 
 * PWM1 - J4
 * PWM2 - J5
 * PWM3 - J6
 * PWM4 - J7 */
#define VPWMSELECT PWM4

/* Current Loop PWM Selection */
/* Select between PWMs 5-6 */
/* Jumper Position 
 * PWM5 - J10
 * PWM6 - J11  */
#define IPWMSELECT PWM5

/* Ramp PWM Selection */
/* Select PWM7 */
/* Jumper position - J15 */
#define RPWMSELECT PWM7

/* Operating Modes */
#define OPENLOOPDEMO 0
#define CLOSELOOPDEMO 1

/* Mode selection */
#define MODE CLOSELOOPDEMO

/* Closed Loop Operation States */
#define IDLE 0
#define SOFTSTART 1
#define NORMAL 2
#define FAULT 3

/* Output Voltage */
#define VOUT 1.65F

/* Soft Start Time */
#define SSTIME 100   /* in ms */

/* Load Switch Settings for Transient Response */
#define PULLDOWN 0
#define PULLUP 1

/* Switch selection */
#define LOADSW PULLDOWN

/* Load Transient Time */
#define LDTRANSTIME 1000 /* in us */
#define LDPERIOD 1000   /* in ms */
#define SELFTESTTIME 2000 /* in ms */
#define DEBOUNCE_DELAY 500 /* in ms */

/* Voltage OV/UV Fault Enable */
#define VLTFLTENABLED OFF /* ON to detect fault, Default:OFF */
#define FLTRESETTIME  1000 /* in ms */
#define RESTARTENABLED OFF /* ON for fault restart, Default:OFF */

/* Define Peak Current Mode */
#define IPEAK OFF

/* Peripheral Usage Subroutines */
void softStartRoutine(int16_t delay, int16_t refVoltage);
uint16_t readR12(void);
uint16_t readR20(void);
void SelfTest(void);
void SwitchDetect(void);
void LoadControl(void);
void BlinkLED(uint8_t varTime);
void StartTimer1(void);
void StopTimer1(void);







