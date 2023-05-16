/*
 * cmu.c
 *
 *  Methods to initialize and configure the BG Clock Management Unit (CMU)
 *
 *  Created on: Jan 26, 2020
 *      Author: Brian I.
 *
 *  Resources Used:
 *    - https:/www.silabs.com/documents/public/reference-manuals/efr32xg1-rm.pdf
 *    - https:/siliconlabs.github.io/Gecko_SDK_Doc/efm32g/html/index.html
 *    - an0004.1-efm32-cmu.pdf
 */
/*-----------------------------------*/
#ifndef SRC_CMU_H_
#define SRC_CMU_H_
/*-----------------------------------*/
#include <stdbool.h>

#include "em_cmu.h"
#include "sleep.h"

#define LFXO_PRESCALER_DIV (cmuClkDiv_2)
/*-----------------------------------*/
/*
 * Initialize the BG's CMU based on the EnergyMode specified. This enables the necessary
 * oscillator, Clock Select, Clock Enable, and Clock Prescaler Divisor, as well as the clock
 * itself.
 *
 * See Figures 12.1 and 12.2 from the link below:
 *  https://www.silabs.com/documents/public/reference-manuals/efr32xg1-rm.pdf
 *
 * @param mode - Energy mode selected for system, used to determine how CMU is initialized
 */
void cmuEnable(SLEEP_EnergyMode_t mode);
/*-----------------------------------*/
#endif /* SRC_CMU_H_ */
