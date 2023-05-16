/*
 * clock.h
 *
 *  Created on: Apr 22, 2020
 *      Author: brian
 *
 *  Methods to setup and interface with the RTCC module. This configures the RTCC module to utilize the system clock in use
 *  and methods to read various fields from the TIME register.
 *
 * Notes and references were used from the following sources:
 * - https://www.silabs.com/community/wireless/bluetooth/knowledge-base.entry.html/2016/10/15/using_rtcc_as_timer-cFRe
 * - https://www.silabs.com/documents/public/application-notes/AN0005-EFM32-RTC.pdf
 * - https://www.silabs.com/documents/public/reference-manuals/efr32xg1-rm.pdf
 *
 */

#ifndef SRC_CLOCK_H_
#define SRC_CLOCK_H_
/*---------------------------------------------------*/
#include <stdint.h>
#include "em_rtcc.h"

#include "sleep.h"

/*---------------------------------------------------*/
/*
 * Initialize RTCC module into calendar mode.
 * This is used to track the current date/time for setting the detector schedule.
 *
 * @param mode - Used to determine which clock to source from
 */
void clockInit(SLEEP_EnergyMode_t mode);
/*---------------------------------------------------*/
/*
 * Return the current hour of the day tracked by the RTCC module.
 */
uint32_t getCurrentHour();
/*---------------------------------------------------*/
/*
 * Return the current hour of day tracked by the RTCC module converted to a char array string.
 *
 * @param hourString - Pointer to char array to be updated with current hour string.
 */
void getCurrentHourStr(char* hourString);
/*---------------------------------------------------*/
/*
 * Return the current time of day tracked by the RTCC module converted to a char array string.
 *
 * @param timeString - Pointer to char array to be updated with current time string as HH:MM:SS.
 */
void getCurrentTimeStr(char* timeString);

/*---------------------------------------------------*/
#endif /* SRC_CLOCK_H_ */
