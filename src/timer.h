/*---------------------------------------------------*/
#ifndef SRC_TIMER_H_
#define SRC_TIMER_H_

#include "em_letimer.h"
#include "sleep.h"

/* Bitmask to specify which interrupt event has triggered */
#define PB0_BUTTON_PUSH_BITMASK (0x0010)
#define PB1_BUTTON_PUSH_BITMASK (0x0020)
#define PIR_SENSOR_BITMASK (0x0040)

/*---------------------------------------------------*/
/* Global Timer Variables */
uint8_t g_eventProcessingBitmask;

/*---------------------------------------------------*/
/*
 * Initialize the BG LETIMER0 based on the energy mode and desired periodic interrupt specified
 * in the #define macros above. Also enables the LETIMER0 NVIC interrupt and enables the LETIMER0 for use.
 *
 */
void timerInit();

/*
 * Disable LETIMER0 and disable interrupt associated with LETIMER0
 */
void timerDisable();

/*
 * Polling delay based on the us_wait parameter specified. This utilizes the LETIMER
 * tick count to give precise and consist timing delays.
 *
 * NOTE: With LFXO, a clock rate of 32768 Hz only allows for a
 *       minimum of ~30 usec resolution for CNT value increment.
 *       With ULFRCO, clock rate of 1000 Hz only allows for
 *       minimum of ~1000 usec resolution for CNT value increment.
 *
 * @param us_wait - microseconds to delay.
 * @param mode - Energy mode selected for system, used to determine which
 *               clk freq to account for when determining delay
 */
void timerWaitUs(uint32_t us_wait);

/*
 * Method to set a one-shot timer event to trigger an interrupt
 * after the specified number of ms_until_wakeup input.
 *
 * @param ms_until_wakeup - ms delay to set before interrupt triggered
 */
void timerSetEventInMs(uint32_t ms_until_wakeup);

/*
 * Sets the current energy mode in use by system to allow timer calculations
 * to be accurate based on oscillator in use.
 *
 * @param mode - the EnergyMode the system is in.
 */
void timerSetCurrentEnergyMode(SLEEP_EnergyMode_t mode);

/*
 * Method to get the current number of msec since system power on
 *
 * @return - Msec since system startup and timer enabled
 */
uint32_t timerGetRunTimeMilliseconds();

/*
 * Initiate EM3 Sleep Mode while waiting for TIMER0 interrupts
 */
void timer0EM3BlockStart();

/*
 * End EM3 Sleep Mode after TIMER0 interrupt complete
 */
void timer0EM3BlockEnd();

/*---------------------------------------------------*/
#endif /* SRC_TIMER_H_ */
